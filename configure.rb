#!/usr/bin/env ruby

BUILD_DIR = 'build'

def read_qtc_version
  output = `qtcreator -version 2>&1`
  found = output =~ /Qt\ Creator (\d\.\d+\.\d+)/
  abort 'Sorry, can not find the QtCreator version.' unless found
  $1
rescue Errno::ENOENT
  abort 'Install QtCreator and/or have it on your system PATH first.'
end

def get_qtc_sources version
  puts 'Downloading QtCreator sources... (using curl)'

  short_version = version.gsub(/\.\d+$/, '')
  Dir.chdir(BUILD_DIR) do
    url = "http://download.qt-project.org/official_releases/qtcreator/#{short_version}/#{version}/qt-creator-opensource-src-#{version}.tar.gz"
    if system("curl -L -O #{url}")
        abort 'Failed to unpack QtCreator sources.' unless system("tar -xf qt-creator-opensource-src-#{version}.tar.gz")
    end
  end
end

qtc_version = read_qtc_version
qtc_dir = "#{BUILD_DIR}/qt-creator-opensource-src-#{qtc_version}"

Dir.mkdir(BUILD_DIR) unless File.exist?(BUILD_DIR)
if File.exist?(qtc_dir)
  puts 'Qt Creator seems to be already downloaded.'
else
  get_qtc_sources(qtc_version)
end

Dir.chdir(BUILD_DIR) do
  system("qmake QTC_SOURCE=#{qtc_dir} ..")
end
