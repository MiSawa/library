#!/usr/bin/env ruby

def measure_time(&block)
  start = Time.now
  block.call
  Time.now - start
end

def test_multipathstory
  measure_time do
    for io in Dir.glob('./multipathstory/in/**').sort.zip(Dir.glob('./multipathstory/out/**').sort)
      puts "---#{io[0].split('/')[-1]}---"
      res = `./a.out < #{io[0]}`.to_i
      expect = open(io[1]).read.to_i
      if res == expect
        puts "AC"
      else
        puts "WA"
      end
    end
  end
end
def test_AOJ2266
  ios = Dir.glob('./cache/*.in').sort.zip(Dir.glob('./cache/*.diff').sort)
  for io in ios
    print "---#{io[0].split('/')[-1]}---"

    t = Time.now
    res = `./a.out < #{io[0]}`.to_i
    puts "   #{format("%.4f", Time.now - t)} sec"

    expect = open(io[1]).read.to_i
    if res == expect
      puts "AC"
    else
      puts "!!!!!!!!!! WA !!!!!!!!!!"
      puts "your output : #{res}"
      puts "expected    : #{expect}"
    end
  end
end
test_AOJ2266
