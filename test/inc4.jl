typecheckArray()="none"
typecheckArray(x::Array{Void,1})="void"
typecheckArray(x::Array{Bool,1})="boolean"
typecheckArray(x::Array{Int64,1})="int"
typecheckArray(x::Array{Float64,1})="float"
typecheckArray(x::Array{String,1})="string"
concat(x::Array{String,1})=reduce((x,y)->x*y,x)
true
