
require 'pp'
class Graph
	attr_reader :verts, :rep
	attr_accessor :no_edge_val
	#Must have these methods defined in child classes
	#graph.edge(i,j)
	#graph.set_edge(i,j,value)
	#graph.set_dir_edge(i,j,value)
	#graph.reorder(permutation)
	#graph.resize(i)
	
	def edge?(i,j)
		if(edge(i,j) != @no_edge_val)
			return true
		end
		return false
	end
	
	def print_adj_mat
		for i in 0...@verts
			for j in 0...@verts
				print "#{edge(i,j)},"
			end
			print "\n"
		end
	
	end
	
	def print_neato
		puts 'graph dasGraph { '
		for i in 0...@verts
			for j in i...@verts
				if(edge(i,j) != @no_edge_val)
					puts("v#{i} -- v#{j}; ") 
				end
			end
		end
		puts ' } '
	end
	def print_dreadnaught
		print "n=#{@verts} g\n"
		for i in 0...@verts
			for j in (i+1)...@verts
				if(edge(i,j) != @no_edge_val)
					print "#{j} " 
				end
			end
			print ";\n"
		end
		print "x\no\n"
	end
	
	def print_brew
		print "#{@verts}\n"
		for i in 0...@verts
			for j in (i+1)...@verts
				if(edge(i,j) != @no_edge_val)
					print "#{i} #{j}\n" 
				end
			end
		end
		print "-1"
	end
	
	def print_dimacs
		edge_count=0
		for i in 0...@verts
			for j in (i+1)...@verts
				if(edge(i,j) != @no_edge_val)
					edge_count+=1 
				end
			end
		end
		print "p edge #{@verts} #{edge_count}\n"
		for i in 0...@verts
			for j in (i+1)...@verts
				if(edge(i,j) != @no_edge_val)
					print "e #{i+1} #{j+1}\n" 
				end
			end
		end
		
	end

	
	
end

#class OutOfCoreDenseGraph
#	def initialize
#	@temp_dir ="/tmp/"
#	end
#end

#Writing to disk for huge sparse graphs
#class OutOfCoreSparseGraph
#end

# An interface for a graph distributed over a MPI network
# Keeps a local cache of changes/requests so it doesn't have to do as many queries
# Also supports batch queries
#class DistributedGraph
#
#end



class SparseGraph < Graph
	def initialize (size)
		@rep=Array.new(size)
		@rep.fill{ |i| 
			Array.new 
		}
		@verts=size
		@no_edge_val=0
	end
	
	def edge(i,j)
		@rep[i].each{ |cur|
			if(cur[0]==j)
				return cur[1]
			end
		}
		return @no_edge_val
	end
	
	def set_dir_edge(i,j,value)
		edge_set=false
		@rep[i].each_index{ |k|
			if(@rep[i][k][0]==j)
				@rep[i][k][1]=value
				edge_set=true
				break
			end
		}
		if(!edge_set)
			@rep[i].push([j,value])
		end
	end
	
	def set_edge(i,j,value)
		set_dir_edge(i,j,value)
		set_dir_edge(j,i,value)
	end
	
	def resize(new_size)
		copy_num  = [new_size, @verts].min
		new_graph=SparseGraph.new(new_size)
		for i in 0...copy_num
			for  j in 0...copy_num
				val= self.edge(i,j)
				if(val != @no_edge_value)
					new_graph.set_dir_edge(i,j,val)
				end
			end
		end
	    @verts=new_size
		@rep=new_graph.rep
	end
	
	def reorder(perm)
		new_graph=DenseGraph.new(@verts)
		for i in 0...(@verts-1)
			for j in 0...(@verts-1)
				val=self.edge(perm[i],perm[j])
				if(val !=@no_edge_value)
					new_graph.set_dir_edge(i,j,val)
				end
			end
		end
	end
	
	
end





class DenseGraph < Graph
	def initialize(size)
		@rep=Array.new(size)
		@rep.fill{ |i| Array.new(size).fill{|j| 0} }
		@verts=size
		@no_edge_val = 0
	end
	
	def edge(i,j)
		return @rep[i][j]
	end

	def set_edge(i,j,value)
		@rep[i][j]=value
		@rep[j][i]=value
	end
	
	def	set_dir_edge(i,j,value)
		@rep[i][j]=value
	end
	
	def reorder(perm)
		newGraph= @rep
		@rep.each_index{|i|
			@rep[i].each_index{|j|
				newGraph[i][j]= self.edge(perm[i],perm[j])
			}
		}
		@rep=newGraph
	end
	
	def resize(size)
		new_rep=Array.new(size)
		new_rep.fill{ |i| Array.new(size).fill{|j| 0} }
		copy_count=[size,@verts].min
		for i in 0...copy_count
			for j in 0...copy_count
				new_rep[i][j] = @rep[i][j]
			end
		end
		@rep=new_rep
		@verts=size
	end
end
