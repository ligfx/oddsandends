module Queens
	class Pos < Struct.new :x, :y
		def ==(other); self.x == other.x and self.y == other.y; end
		def conflicts(other);
			self.x == other.x or
			self.y == other.y or
			(self.y - other.y).abs == (self.x - other.x).abs
		end
	end
	class State
		def initialize opts={}
			@state = opts[:state] || []
			@moves = opts[:moves] || (1..8).to_a.repeated_permutation(2).map { |a, b| Pos.new(a,b) }
		end
		attr_reader :state
		def each
			while move = @moves.shift
				new_moves = @moves.find_all { |p| not move.conflicts(p) }
				yield self.class.new :state => (@state + [move]), :moves => new_moves
			end
		end
		def valid?
			return state.size == 8 ? :correct : :incomplete
		end
	end
end

require 'pp'
s = Queens::State.new
$COUNTER = 0
def pretty(state)
	board = Array.new(8) { Array.new(8) {" "}}
	state.each do |pos|
		board[pos.y - 1][pos.x - 1] = "X"
	end
	board.map { |row| "|" + row.join("|") + "|" }.join("\n")
end
def find(s)
	s.each do |p|
		case p.valid?
		when :incomplete
			find(p)
		when :correct
			puts "-"*10
			$COUNTER += 1
			puts "Solution! #{$COUNTER}/92"
			puts pretty p.state
		end
	end
end
find s