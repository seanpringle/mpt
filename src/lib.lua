
v2 = (function()

	local v2lib = {
		add = function(va,vb)
			return v2(va[1]+vb[1], va[2]+vb[2])
		end,
	}

	return function(x,y)
		return setmetatable({x,y},{__index = v2lib})
	end
end)()

v3,rgb = (function()

	local v3lib = {
		add = function(va,vb)
			return v3(va[1]+vb[1], va[2]+vb[2], va[3]+vb[3])
		end,
		scale = function(va,mul)
			return v3(va[1]*mul, va[2]*mul, va[3]*mul)
		end,
	}

	local rgblib = {
		shine = function(rgb,mul)
			return rgb:scale(mul)
		end,
	}

	setmetatable(rgblib, {__index = v3lib})

	return
		function(x,y,z)
			return setmetatable({x,y,z},{__index = v3lib})
		end,
		function(r,g,b)
			return setmetatable({r,g,b},{__index = rgblib})
		end
end)()

white  = rgb(1,1,1)
red    = rgb(1,0,0)
green  = rgb(0,1,0)
blue   = rgb(0,0,1)
black  = rgb(0.001,0.001,0.001)
naught = rgb(0,0,0)

zero2 = v2(0,0)
zero3 = v3(0,0,0)

X = v3(1,0,0)
Y = v3(0,1,0)
Z = v3(0,0,1)

function scene(t)
	local width     = t.width     or 1280
	local height    = t.height    or 720
	local passes    = t.passes    or 0
	local bounces   = t.bounces   or 10
	local seed      = t.seed      or 123456789
	local horizon   = t.horizon   or 100000
	local threshold = t.threshold or 0.0001
	local ambient   = t.ambient   or naught
	local path      = t.path      or "out.png"

	for i = 1,#args do
		if args[i] == "--width" then
			width = tonumber(args[i+1])
		end
		if args[i] == "--height" then
			height = tonumber(args[i+1])
		end
		if args[i] == "--passes" then
			passes = tonumber(args[i+1])
		end
		if args[i] == "--bounces" then
			bounces = tonumber(args[i+1])
		end
		if args[i] == "--seed" then
			seed = tonumber(args[i+1])
		end
		if args[i] == "--horizon" then
			horizon = tonumber(args[i+1])
		end
		if args[i] == "--threshold" then
			threshold = tonumber(args[i+1])
		end
		if args[i] == "--path" then
			path = args[i+1]
		end
	end

	_scene(
		width,
		height,
		passes,
		bounces,
		seed,
		horizon,
		threshold,
		ambient,
		path
	)
end

function perspective(t)
	_perspective(
		t.from     or v3(0,-1,-1),
		t.look     or zero3,
		t.up       or Z,
		t.fov      or 40,
		t.focus    or zero3,
		t.aperture or 0
	)
end

function rotateX(deg, sdf)
	return rotate(X, deg, sdf)
end

function rotateY(deg, sdf)
	return rotate(Y, deg, sdf)
end

function rotateZ(deg, sdf)
	return rotate(Z, deg, sdf)
end

function translateX(dist, sdf)
	return translate(v3(dist,0,0), sdf)
end

function translateY(dist, sdf)
	return translate(v3(0,dist,0), sdf)
end

function translateZ(dist, sdf)
	return translate(v3(0,0,dist), sdf)
end

function cubeR(x, y, z, r)
	return rounded(r, cube(x-r*2, y-r*2, z-r*2))
end

function cylinderR(h, d, r)
	return rounded(r, cylinder(h-r*2, d-r*2))
end

steel     = metal(rgb(0.4, 0.4, 0.4), 0.95)
stainless = metal(rgb(0.4, 0.4, 0.4), 0.3)
gold      = metal(rgb(0.93, 0.78, 0.31), 0.0)
copper    = metal(rgb(0.68, 0.45, 0.41), 0.8)
brass     = metal(rgb(0.80, 0.58, 0.45), 0.9)

function workbench(size)
	object(
		matt(rgb(0.16, 0.12, 0.09)),
		translate(v3(0, 0, -5), cube(size, size, 10))
	)
end

