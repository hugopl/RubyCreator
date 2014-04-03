require 'parser/current'
require 'stringio'

def tokenize(data)
    parser = Parser::CurrentRuby.new

    parser.diagnostics.all_errors_are_fatal = false
    parser.diagnostics.consumer = lambda do |diagnostic|
        loc = diagnostic.location
#         p "#{diagnostic.level} #{loc.line}:#{loc.column}-#{loc.length} #{diagnostic.message}"
    end
    buffer = Parser::Source::Buffer.new('')
    buffer.source = data

    return parser.tokenize(buffer)[2]
end

def all_tokens(data)
    buffer = Parser::Source::Buffer.new('')
    buffer.source = data

    tokens = $parser.tokenize(buffer)[2]
    tokens.each do |tokenInfo|
        token = tokenInfo[0]
        range = tokenInfo[1][1]
        next if token === :tNL
        puts("#{token} #{range.line}:#{range.column}-#{range.length}")
    end
end

# still ignoring current class, module, etc.
def get_method_declarations(data)
    result = StringIO.new
    is_def_identifier = false
    tokenize(data).each do |tokenInfo|
        if tokenInfo[0] === :kDEF
            is_def_identifier = true
        elsif is_def_identifier
            range = tokenInfo[1][1]
            result << "#{range.line} #{range.column} #{tokenInfo[1][0]}\n"
            is_def_identifier = false
        end
    end
    return result.string
end
