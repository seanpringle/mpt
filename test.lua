
scene({
	width     = 1920,
	height    = 1080,
	passes    = 0,
	bounces   = 16,
	seed      = 123456789,
	horizon   = 100000,
	threshold = 0.0001,
	ambient   = white:scale(0.05),
})

perspective({
	from     = v3(0, -8000, 8000),
	look     = v3(0, -1000, 500),
	up       = v3(0, 0, 1),
	fov      = 40,
	focus    = zero3,
	aperture = 0.0,
})

workbench(25000)

object(
	light(white:shine(4)),
	translate(v3(-7500, 0, 20000), sphere(20000))
)

object(brass,
	translate(v3(-5500, -1200, 500),
		subtract({
			intersect({
				sphere(1000),
				cube(800, 800, 800),
			}),
			cylinder(1002, 400),
			rotateX(90, cylinder(1002, 400)),
			rotateY(90, cylinder(1002, 400)),
		})
	)
)

object(brass,
	translate(v3(5500, -1200, 500),
		combine({
			intersect({
				sphere(1000),
				cube(800, 800, 800)
			}),
			cylinder(1250, 400),
			rotateX(90, cylinder(1250, 400)),
			rotateY(90, cylinder(1250, 400))
		})
	)
)

object(brass,
	translate(v3(6500, 3750, 500),
		rounded(100, cube(800, 800, 800))
	)
)

object(brass,
	translate(v3(-6500, 3750, 500),
		rounded(100, cylinder(800, 1000))
	)
)

object(steel,
	translate(v3(6000, 900, 600),
		reprise(1, 1, 1, 400, 400, 400, sphere(400))
	)
)

object(copper,
	translate(v3(-6000, 900, 500),
		capsule(750, 1000, 500)
	)
)

bowl = revolve(0, parabola(2000, 1500))

object(
	glass(rgb(1.0, 0.5, 0.5), 1.5),
	combine({
		translateZ(100, cylinderR(200, 1000, 10)),
		subtract({bowl, translateZ(200, bowl)})
	})
)

for i = 0,8,1 do
	object(glass(white, 1.5),
		translateY(-100, rotateZ(i*45, translate(v3(700, 0, 100), sphere(200))))
	)
end

function things(relative, material)
	object(material,
		translate(relative:add(v3(0,0,500)), sphere(1000))
	)

	object(material,
		translate(relative:add(v3(0,2000,500)), cube(1000, 1000, 1000))
	)

	object(material,
		translate(relative:add(v3(-1500,0,500)), cylinder(1000, 1000))
	)

	object(material,
		translate(relative:add(v3(1500,0,600)), rotateX(90, torus(1000, 200)))
	)

	object(material,
		translate(relative:add(v3(-1500,2000,500)), pyramid(1000, 1000))
	)

	object(material,
		translate(relative:add(v3(1500,2000,500)), cone(1000, 1000))
	)

	for i = 3,8,1 do
		object(material,
			translate(relative:add(v3((i-3)*600-1450, -1250, 250)), extrude(500, polygon(i, 500)))
		)
	end
end

things(v3(-2750, -2750, 0), steel)
things(v3(2750, -2750, 0), copper)
things(v3(-3000, 2250, 0), stainless)
things(v3(3000, 2250, 0), gold)

for i = 0,9,1 do
	if not (i >= 3 and i < 6) then
		local color = (i%2 == 0) and rgb(0.5, 1.0, 0.5) or rgb(0.5, 0.5, 1.0)
		object(glass(color, 1.5),
			translate(v3(0, i*1000-4000, 250), sphere(500))
		)
	end
end
