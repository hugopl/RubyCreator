# wraper for rubocop
begin
  require 'rubocop'

  $rubocop36 = Gem::Version.new(RuboCop::Version.version) >= Gem::Version.new('0.36.0')
rescue LoadError
  abort 'Rubocop not installed or not found, if you use rvm, check if rubocop is installed on your system ruby.'
end

module RuboCop
  class Runner
    if $rubocop36
      def _inspect_code(code, path)
        @formatter_set ||= Formatter::FormatterSet.new
        # This would be configurable in the project settings someday
        inspect_file(RuboCop::ProcessedSource.new(code, RUBY_VERSION.to_f, path))[0]
      end
    else
      def _inspect_code(code, path)
        @formatter_set ||= Formatter::FormatterSet.new
        inspect_file(RuboCop::ProcessedSource.new(code, path))[0]
      end
    end
  end
end

class RouboCop
  def initialize
    config = RuboCop::ConfigStore.new
    options, _ = RuboCop::Options.new.parse([])
    @runner = RuboCop::Runner.new(options, config)
  end

  def parse(data)
    idx = data.index("\n")
    path = data[0, idx]
    file = data[(idx + 1)..-2]
    file.gsub!("\n", "\r\n") if RUBY_PLATFORM.include?('mingw')
    offenses = @runner._inspect_code(file, path)

    offenses.sort! { |a, b| a.location.line <=> b.location.line }
    offenses.each do |offense|
      location = offense.location
      puts "#{offense.severity.code}:#{location.line}:#{location.column + 1}:#{location.length}:#{offense.message.tr("\n", ' ')} [#{offense.cop_name}]"
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
  until $stdin.eof?
    data = $stdin.gets("\0")
    roubocop.parse(data)
  end
end

main
