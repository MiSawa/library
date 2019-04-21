#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>

typedef unsigned int uint;
typedef long int lint;
typedef long long int llint;
typedef unsigned long long int ullint;

struct node;

typedef struct arc {
  struct node *from;
  struct node *to;
  uint flow;
  uint capacity;
  uint direction;
} Arc;

typedef struct node {
  uint numAdjacent;
  uint number;
  uint label;
  int excess;
  struct node *parent;
  struct node *childList;
  struct node *nextScan;
  uint numOutOfTree;
  Arc **outOfTree;
  uint nextArc;
  Arc *arcToParent;
  struct node *next;
} Node;

typedef struct root {
  Node *start;
  Node *end;
} Root;

//---------------  Global variables ------------------
static uint numNodes = 0;
static uint numArcs = 0;
static uint source = 0;
static uint sink = 0;

static uint highestStrongLabel = 1;

static Node *adjacencyList = NULL;
static Root *strongRoots = NULL;
static uint *labelCount = NULL;
static Arc *arcList = NULL;
//-----------------------------------------------------

#ifdef STATS
static ullint numPushes = 0;
static uint numMergers = 0;
static uint numRelabels = 0;
static uint numGaps = 0;
static ullint numArcScans = 0;
#endif

static void initializeNode(Node *nd, const uint n) {
  nd->label = 0;
  nd->excess = 0;
  nd->parent = NULL;
  nd->childList = NULL;
  nd->nextScan = NULL;
  nd->nextArc = 0;
  nd->numOutOfTree = 0;
  nd->arcToParent = NULL;
  nd->next = NULL;
  nd->numAdjacent = 0;
  nd->number = n;
  nd->outOfTree = NULL;
}

static void initializeRoot(Root *rt) {
  rt->start = NULL;
  rt->end = NULL;
}

static void freeRoot(Root *rt) {
  rt->start = NULL;
  rt->end = NULL;
}

static void liftAll(Node *rootNode) {
  Node *temp, *current = rootNode;

  current->nextScan = current->childList;

  --labelCount[current->label];
  current->label = numNodes;

  for (; (current); current = current->parent) {
    while (current->nextScan) {
      temp = current->nextScan;
      current->nextScan = current->nextScan->next;
      current = temp;
      current->nextScan = current->childList;

      --labelCount[current->label];
      current->label = numNodes;
    }
  }
}

static void addToStrongBucket(Node *newRoot, Root *rootBucket) {
  if (rootBucket->start) {
    rootBucket->end->next = newRoot;
    rootBucket->end = newRoot;
    newRoot->next = NULL;
  } else {
    rootBucket->start = newRoot;
    rootBucket->end = newRoot;
    newRoot->next = NULL;
  }
}

static void createOutOfTree(Node *nd) {
  if (nd->numAdjacent) {
    if ((nd->outOfTree = (Arc **)malloc(nd->numAdjacent * sizeof(Arc *))) ==
        NULL) {
      printf("%s Line %d: Out of memory\n", __FILE__, __LINE__);
      exit(1);
    }
  }
}

static void initializeArc(Arc *ac) {
  ac->from = NULL;
  ac->to = NULL;
  ac->capacity = 0;
  ac->flow = 0;
  ac->direction = 1;
}

static void addOutOfTreeNode(Node *n, Arc *out) {
  n->outOfTree[n->numOutOfTree] = out;
  ++n->numOutOfTree;
}

static void readDimacsFileCreateList(void) {
  uint lineLength = 1024, i, capacity, numLines = 0, from, to, first = 0,
       last = 0;
  char *line, *word, ch, ch1;

  if ((line = (char *)malloc((lineLength + 1) * sizeof(char))) == NULL) {
    printf("%s, %d: Could not allocate memory.\n", __FILE__, __LINE__);
    exit(1);
  }

  if ((word = (char *)malloc((lineLength + 1) * sizeof(char))) == NULL) {
    printf("%s, %d: Could not allocate memory.\n", __FILE__, __LINE__);
    exit(1);
  }

  while (fgets(line, lineLength, stdin)) {
    ++numLines;

    switch (*line) {
    case 'p':

      sscanf(line, "%c %s %d %d", &ch, word, &numNodes, &numArcs);

      if ((adjacencyList = (Node *)malloc(numNodes * sizeof(Node))) == NULL) {
        printf("%s, %d: Could not allocate memory.\n", __FILE__, __LINE__);
        exit(1);
      }

      if ((strongRoots = (Root *)malloc(numNodes * sizeof(Root))) == NULL) {
        printf("%s, %d: Could not allocate memory.\n", __FILE__, __LINE__);
        exit(1);
      }

      if ((labelCount = (uint *)malloc(numNodes * sizeof(uint))) == NULL) {
        printf("%s, %d: Could not allocate memory.\n", __FILE__, __LINE__);
        exit(1);
      }

      if ((arcList = (Arc *)malloc(numArcs * sizeof(Arc))) == NULL) {
        printf("%s, %d: Could not allocate memory.\n", __FILE__, __LINE__);
        exit(1);
      }

      for (i = 0; i < numNodes; ++i) {
        initializeRoot(&strongRoots[i]);
        initializeNode(&adjacencyList[i], (i + 1));
        labelCount[i] = 0;
      }

      for (i = 0; i < numArcs; ++i) {
        initializeArc(&arcList[i]);
      }

      first = 0;
      last = numArcs - 1;

      break;

    case 'a':

      sscanf(line, "%c %d %d %d", &ch, &from, &to, &capacity);

      if ((from + to) % 2) {
        arcList[first].from = &adjacencyList[from - 1];
        arcList[first].to = &adjacencyList[to - 1];
        arcList[first].capacity = capacity;

        ++first;
      } else {
        arcList[last].from = &adjacencyList[from - 1];
        arcList[last].to = &adjacencyList[to - 1];
        arcList[last].capacity = capacity;
        --last;
      }

      ++adjacencyList[from - 1].numAdjacent;
      ++adjacencyList[to - 1].numAdjacent;

      break;

    case 'n':

      sscanf(line, "%c %d %c", &ch, &i, &ch1);

      if (ch1 == 's') {
        source = i;
      } else if (ch1 == 't') {
        sink = i;
      } else {
        printf("Unrecognized character %c on line %d\n", ch1, numLines);
        exit(1);
      }

      break;
    }
  }

  for (i = 0; i < numNodes; ++i) {
    createOutOfTree(&adjacencyList[i]);
  }

  for (i = 0; i < numArcs; i++) {
    to = arcList[i].to->number;
    from = arcList[i].from->number;
    capacity = arcList[i].capacity;

    if (!((source == to) || (sink == from) || (from == to))) {
      if ((source == from) && (to == sink)) {
        arcList[i].flow = capacity;
      } else if (from == source) {
        addOutOfTreeNode(&adjacencyList[from - 1], &arcList[i]);
      } else if (to == sink) {
        addOutOfTreeNode(&adjacencyList[to - 1], &arcList[i]);
      } else {
        addOutOfTreeNode(&adjacencyList[from - 1], &arcList[i]);
      }
    }
  }

  free(line);
  line = NULL;

  free(word);
  word = NULL;
}

static void simpleInitialization(void) {
  uint i, size;
  Arc *tempArc;

  size = adjacencyList[source - 1].numOutOfTree;
  for (i = 0; i < size; ++i) {
    tempArc = adjacencyList[source - 1].outOfTree[i];
    tempArc->flow = tempArc->capacity;
    tempArc->to->excess += tempArc->capacity;
  }

  size = adjacencyList[sink - 1].numOutOfTree;
  for (i = 0; i < size; ++i) {
    tempArc = adjacencyList[sink - 1].outOfTree[i];
    tempArc->flow = tempArc->capacity;
    tempArc->from->excess -= tempArc->capacity;
  }

  adjacencyList[source - 1].excess = 0;
  adjacencyList[sink - 1].excess = 0;

  for (i = 0; i < numNodes; ++i) {
    if (adjacencyList[i].excess > 0) {
      adjacencyList[i].label = 1;
      ++labelCount[1];

      addToStrongBucket(&adjacencyList[i], &strongRoots[1]);
    }
  }

  adjacencyList[source - 1].label = numNodes;
  adjacencyList[sink - 1].label = 0;
  labelCount[0] = (numNodes - 2) - labelCount[1];
}

static inline int addRelationship(Node *newParent, Node *child) {
  child->parent = newParent;
  child->next = newParent->childList;
  newParent->childList = child;

  return 0;
}

static inline void breakRelationship(Node *oldParent, Node *child) {
  Node *current;

  child->parent = NULL;

  if (oldParent->childList == child) {
    oldParent->childList = child->next;
    child->next = NULL;
    return;
  }

  for (current = oldParent->childList; (current->next != child);
       current = current->next)
    ;

  current->next = child->next;
  child->next = NULL;
}

static void merge(Node *parent, Node *child, Arc *newArc) {
  Arc *oldArc;
  Node *current = child, *oldParent, *newParent = parent;

#ifdef STATS
  ++numMergers;
#endif

  while (current->parent) {
    oldArc = current->arcToParent;
    current->arcToParent = newArc;
    oldParent = current->parent;
    breakRelationship(oldParent, current);
    addRelationship(newParent, current);
    newParent = current;
    current = oldParent;
    newArc = oldArc;
    newArc->direction = 1 - newArc->direction;
  }

  current->arcToParent = newArc;
  addRelationship(newParent, current);
}

static inline void pushUpward(Arc *currentArc, Node *child, Node *parent,
                              const uint resCap) {
#ifdef STATS
  ++numPushes;
#endif

  if (resCap >= child->excess) {
    parent->excess += child->excess;
    currentArc->flow += child->excess;
    child->excess = 0;
    return;
  }

  currentArc->direction = 0;
  parent->excess += resCap;
  child->excess -= resCap;
  currentArc->flow = currentArc->capacity;
  parent->outOfTree[parent->numOutOfTree] = currentArc;
  ++parent->numOutOfTree;
  breakRelationship(parent, child);

  addToStrongBucket(child, &strongRoots[child->label]);
}

static inline void pushDownward(Arc *currentArc, Node *child, Node *parent,
                                uint flow) {
#ifdef STATS
  ++numPushes;
#endif

  if (flow >= child->excess) {
    parent->excess += child->excess;
    currentArc->flow -= child->excess;
    child->excess = 0;
    return;
  }

  currentArc->direction = 1;
  child->excess -= flow;
  parent->excess += flow;
  currentArc->flow = 0;
  parent->outOfTree[parent->numOutOfTree] = currentArc;
  ++parent->numOutOfTree;
  breakRelationship(parent, child);

  addToStrongBucket(child, &strongRoots[child->label]);
}

static void pushExcess(Node *strongRoot) {
  Node *current, *parent;
  Arc *arcToParent;
  int prevEx = 1;

  for (current = strongRoot; (current->excess && current->parent);
       current = parent) {
    parent = current->parent;
    prevEx = parent->excess;

    arcToParent = current->arcToParent;

    if (arcToParent->direction) {
      pushUpward(arcToParent, current, parent,
                 (arcToParent->capacity - arcToParent->flow));
    } else {
      pushDownward(arcToParent, current, parent, arcToParent->flow);
    }
  }

  if ((current->excess > 0) && (prevEx <= 0)) {

    addToStrongBucket(current, &strongRoots[current->label]);
  }
}

static Arc *findWeakNode(Node *strongNode, Node **weakNode) {
  uint i, size;
  Arc *out;

  size = strongNode->numOutOfTree;

  for (i = strongNode->nextArc; i < size; ++i) {

#ifdef STATS
    ++numArcScans;
#endif

    if (strongNode->outOfTree[i]->to->label == (highestStrongLabel - 1)) {
      strongNode->nextArc = i;
      out = strongNode->outOfTree[i];
      (*weakNode) = out->to;
      --strongNode->numOutOfTree;
      strongNode->outOfTree[i] =
          strongNode->outOfTree[strongNode->numOutOfTree];
      return (out);
    } else if (strongNode->outOfTree[i]->from->label ==
               (highestStrongLabel - 1)) {
      strongNode->nextArc = i;
      out = strongNode->outOfTree[i];
      (*weakNode) = out->from;
      --strongNode->numOutOfTree;
      strongNode->outOfTree[i] =
          strongNode->outOfTree[strongNode->numOutOfTree];
      return (out);
    }
  }

  strongNode->nextArc = strongNode->numOutOfTree;

  return NULL;
}

static void checkChildren(Node *curNode) {
  for (; (curNode->nextScan); curNode->nextScan = curNode->nextScan->next) {
    if (curNode->nextScan->label == curNode->label) {
      return;
    }
  }

  --labelCount[curNode->label];
  ++curNode->label;
  ++labelCount[curNode->label];

#ifdef STATS
  ++numRelabels;
#endif

  curNode->nextArc = 0;
}

static void processRoot(Node *strongRoot) {
  Node *temp, *strongNode = strongRoot, *weakNode;
  Arc *out;

  strongRoot->nextScan = strongRoot->childList;

  if ((out = findWeakNode(strongRoot, &weakNode))) {
    merge(weakNode, strongNode, out);
    pushExcess(strongRoot);
    return;
  }

  checkChildren(strongRoot);

  while (strongNode) {
    while (strongNode->nextScan) {
      temp = strongNode->nextScan;
      strongNode->nextScan = strongNode->nextScan->next;
      strongNode = temp;
      strongNode->nextScan = strongNode->childList;

      if ((out = findWeakNode(strongNode, &weakNode))) {
        merge(weakNode, strongNode, out);
        pushExcess(strongRoot);
        return;
      }

      checkChildren(strongNode);
    }

    if ((strongNode = strongNode->parent)) {
      checkChildren(strongNode);
    }
  }

  addToStrongBucket(strongRoot, &strongRoots[strongRoot->label]);

  ++highestStrongLabel;
}

static Node *getHighestStrongRoot(void) {
  uint i;
  Node *strongRoot;

  for (i = highestStrongLabel; i > 0; --i) {
    if (strongRoots[i].start) {
      highestStrongLabel = i;
      if (labelCount[i - 1]) {
        strongRoot = strongRoots[i].start;
        strongRoots[i].start = strongRoot->next;
        strongRoot->next = NULL;
        return strongRoot;
      }

      while (strongRoots[i].start) {

#ifdef STATS
        ++numGaps;
#endif
        strongRoot = strongRoots[i].start;
        strongRoots[i].start = strongRoot->next;
        liftAll(strongRoot);
      }
    }
  }

  if (!strongRoots[0].start) {
    return NULL;
  }

  while (strongRoots[0].start) {
    strongRoot = strongRoots[0].start;
    strongRoots[0].start = strongRoot->next;
    strongRoot->label = 1;
    --labelCount[0];
    ++labelCount[1];

#ifdef STATS
    ++numRelabels;
#endif

    addToStrongBucket(strongRoot, &strongRoots[strongRoot->label]);
  }

  highestStrongLabel = 1;

  strongRoot = strongRoots[1].start;
  strongRoots[1].start = strongRoot->next;
  strongRoot->next = NULL;

  return strongRoot;
}

static void pseudoflowPhase1(void) {
  Node *strongRoot;
  while ((strongRoot = getHighestStrongRoot())) {
    processRoot(strongRoot);
  }
}

static void freeMemory(void) {
  uint i;

  for (i = 0; i < numNodes; ++i) {
    freeRoot(&strongRoots[i]);
  }

  free(strongRoots);

  for (i = 0; i < numNodes; ++i) {
    if (adjacencyList[i].outOfTree) {
      free(adjacencyList[i].outOfTree);
    }
  }

  free(adjacencyList);

  free(labelCount);

  free(arcList);
}

int main(int argc, char **argv) {
  double readStart, readEnd, initStart, initEnd, solveStart, solveEnd,
      flowStart, flowEnd;
  uint gap;

  printf("c Highest label pseudoflow algorithm (Version 3.23)\n");

  printf("c Using FIFO buckets\n");

  readDimacsFileCreateList();

  printf("c Finished reading file.\n");
  fflush(stdout);

  initStart = readEnd;
  simpleInitialization();

  printf("c Finished initialization.\n");
  fflush(stdout);

  solveStart = initEnd;
  pseudoflowPhase1();

  printf("c Finished phase 1.\n");
  fflush(stdout);

  //#ifdef LOWEST_LABEL
  //	gap = lowestStrongLabel;
  //#else
  //	gap = numNodes;
  //#endif

  flowStart = solveEnd;
  // recoverFlow(gap);

  printf("c Number of nodes     : %d\n", numNodes);
  printf("c Number of arcs      : %d\n", numArcs);
  printf("c Time to read        : %.3f\n", (readEnd - readStart));
  printf("c Time to initialize  : %.3f\n", (initEnd - initStart));
  printf("c Time to min cut     : %.3f\n", (solveEnd - initStart));
  printf("c Time to max flow    : %.3f\n", (flowEnd - initStart));
#ifdef STATS
  printf("c Number of arc scans : %lld\n", numArcScans);
  printf("c Number of mergers   : %d\n", numMergers);
  printf("c Number of pushes    : %lld\n", numPushes);
  printf("c Number of relabels  : %d\n", numRelabels);
  printf("c Number of gaps      : %d\n", numGaps);
#endif

  freeMemory();

  return 0;
}
