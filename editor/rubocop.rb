begin
  require 'rubocop'
rescue LoadError
  abort 'Rubocop not installed or not found, if you use rvm, check if rubocop is installed on your system ruby.'
end

module RuboCop
  class Runner
    def _inspect_code(code)
      @formatter_set ||= Formatter::FormatterSet.new
      inspect_file(RuboCop::ProcessedSource.new(code, '/'))[0]
    end
  end
end

class RouboCop
  def initialize
    config = RuboCop::ConfigStore.new
    options, _ = RuboCop::Options.new.parse(%w(-l))
    @runner = RuboCop::Runner.new(options, config)
  end
  
  def parse(file)
    offenses = @runner._inspect_code(file)

    offenses.sort! { |a, b| a.location.line <=> b.location.line }
    offenses.each do |offense|
      location = offense.location
      puts "#{offense.severity.code}:#{location.line}:#{location.column + 1}:#{location.length}:#{offense.message}"
    end
  rescue Errno::ENOENT
    $stderr.puts $!.message
  ensure
    puts '--'
    $stdout.flush
  end
end

def main
  roubocop = RouboCop.new
  while !$stdin.eof?
    data = $stdin.gets("\0")
    roubocop.parse(data)
  end
end

main
