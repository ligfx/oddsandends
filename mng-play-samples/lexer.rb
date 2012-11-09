def skipws(data)
	while data.first =~ /\s/
		data.shift
	end
end

def take_line(data)
	ret = []
	while data.first != "\n"
		ret.push data.shift
	end
	data.shift # \n
	return ret.join
end

def take_digits(data)
	ret = []
	raise unless data.first =~ /\d/
	while data.first =~ /\d/
		ret.push data.shift
	end
	return ret.join
end

def match(data, char)
	if data.first == char
		data.shift
		return true
	else
		return false
	end
end

def take_number(data)
	first = take_digits(data)
	if match(data, '.')
		second = take_digits(data)
		return "#{first}.#{second}".to_f
	else
		return first.to_i
	end
end

def take_word(data)
	raise if ! data.first =~ /[A-Za-z]/
	ret = [data.shift]
	while data.first =~ /[A-Za-z0-9]/
		ret.push data.shift
	end
	return ret.join
end

def lex(data)
	skipws(data)

	return :equals if match(data, '=')
	return :comma if match(data, ',')
	return :lbrace if match(data, '{')
	return :rbrace if match(data, '}')
	return :lparen if match(data, '(')
	return :rparen if match(data, ')')

	if match(data, '/')
		if match(data, '/')
			return [:comment, take_line(data)]
		else
			raise
		end
	elsif match(data, '-')
		return [:number, -take_number(data)]
	elsif data.first =~ /\d/
		return [:number, take_number(data)]
	elsif data.first =~ /[A-Za-z]/
		return [:word, take_word(data)]
	else
		raise
	end
end

def parse_expression(tokens)
	if tokens.first[0] == :word
		name = tokens.shift[1]
		if tokens.first == :lparen or tokens.first == :lbrace
			return parse_function_core(tokens, name)
		else
			return [:Name, name]
		end
	elsif tokens.first[0] == :number
		return [:Constant, tokens.shift[1]]
	else
		raise
	end
end

def parse_function_core(tokens, name)
	if tokens.first == :equals
		tokens.shift # :equals
		rvalue = parse_expression(tokens)

		return [:Assignment, name, rvalue]
	end

	if tokens.first == :lparen
		tokens.shift # :lparen
		args = []
		args.push parse_expression(tokens)
		while tokens.first == :comma
			tokens.shift # :comma
			args.push parse_expression(tokens)
		end
		
		raise unless tokens.shift == :rparen
	else
		args = nil
	end

	if tokens.first == :lbrace
		tokens.shift # :lbrace

		block = []
		while tokens.first.is_a? Array and tokens.first[0] == :word
			block.push parse_function(tokens)
		end

		raise unless tokens.shift == :rbrace
	else
		block = nil
	end

	if args.nil? and block.nil?
		raise
	end

	return [:Function, name, args, block]
end

def parse_function(tokens)
	raise unless tokens.first[0] == :word
	name = tokens.shift[1]

	parse_function_core(tokens, name)	
end

f = File.read(ARGV.shift).chars.to_a

require 'pp'
tokens = []
while f.any?
	tokens.push lex(f)
end
tokens.reject! { |t| t.is_a? Array and t[0] == :comment }

pp tokens
exit

p = Parser.new
nodes = []
tokens.each do |t|
	p.accept(t) do |n| nodes.push n end
end
pp nodes