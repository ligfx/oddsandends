n = 8
cols = (0..7)
cols.to_a.permutation.each { |vec| if (n == (cols.map { |i| vec[i]+i }.uniq.length) and n == (cols.map { |i| vec[i]-1}.uniq.length)); puts "Solution"; end }
