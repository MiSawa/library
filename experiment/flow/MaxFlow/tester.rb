#!/usr/bin/env ruby

InputFileName = './.in'
Exec = './a.out'

def random_tester(t = 100)
  t.times do |casenum|
    puts casenum
    File.open(InputFileName, 'w'){|f|
      $stdout = f
      gen
      $stdout = STDOUT
    }
    a = `#{Exec} #{InputFileName}`
    break unless $?.exitstatus.zero?
  end
end

def gen
  max_n = 5
  max_m = 30
  max_c = 20
  n = rand(max_n)+2
  m = rand(max_m)+1
  puts "p max #{n} #{m}"
  s, t = (1..n+1).to_a.sample(2)
  puts "n #{s} s"
  puts "n #{t} t"
  m.times do
    puts "a #{rand(n)+1} #{rand(n)+1} #{rand(max_c + 1)}"
  end
end
# file_tester
random_tester 50000

