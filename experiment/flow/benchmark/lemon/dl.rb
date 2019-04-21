#!/usr/bin/env ruby

require 'uri'
require 'open-uri'
require 'thread'
require 'parallel'
require 'fileutils'

class ParallelDownloader
  def initialize(options)
    @q           = Queue.new
    @q_mutex     = Mutex.new
    @file_mutex  = Mutex.new
    @print_mutex = Mutex.new
    @th = Thread.start do
      Parallel.each(@q, options) do |job|
        raise Parallel::Break if job == Parallel::Stop
        job.call
      end
    end
  end
  def join
    @q << Parallel::Stop
    @th.join
  end
  def download(url, filename)
    @q_mutex.synchronize{ @q.push Proc.new{
      @file_mutex.synchronize{
        FileUtils.mkdir_p(File.dirname(filename))
      }
      @print_mutex.synchronize{
        puts "downloading #{filename}..."
      }
      open(filename, 'wb').write open(url).read unless FileTest.exist? filename
      @print_mutex.synchronize{
        puts "downloading #{filename} is successfully finished."
      }
    }}
  end
end

famillies = {
  'netgen'     => ['8', 'sr', 'lo_8', 'lo_sr', 'deg', ],
  'gridgen'    => ['8', 'sr', 'deg', ],
  'goto'       => ['8', 'sr', ],
  'gridgraph'  => ['wide', 'long', 'square', ],
  'road'       => ['flow', 'paths', ],
  'vision'     => ['rnd', 'prop', 'inv'],
}

pq = ParallelDownloader.new(:in_threads => 10)

for family, params in famillies
  base = "http://lime.cs.elte.hu/~kpeter/data/mcf/#{family}/"
  open(base).read.scan(/a href="([^~?"]+)".+ ([0-9.]+[KM])/).each do |instance, size|
    if size[-1] == 'K'
      size = size[0...-1] + ' * 1024'
    elsif size[-1] == 'M'
      size = size[0...-1] + ' * 1024 * 1024'
    end
    size = eval(size)
    next if size > 10 * (2**20) # 1M

    url = base + instance

    param = params.select{|param| instance.start_with? family + '_' + param}.max_by(&:length)
    dir = "./#{family}/#{param}/"
    path = dir + instance
    next if FileTest.exist? path

    pq.download(url, path)
  end
end
pq.join

Dir.glob('./gridgraph/grid_*.gz').each do |file|
  param = file.scan(/grid_([^_]+)_*/).first.first
  dir   = "./gridgraph/#{param}"
  FileUtils.mkdir_p(dir)
  FileUtils.move(file, dir)
end
