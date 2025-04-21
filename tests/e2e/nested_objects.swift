class Vector {
    var x = 0
    var y = 0

    func add(x: Int, y: Int) {
        self.x = self.x + x
        self.y = self.y + y
    }
}

class Object {
    var position = Vector()
    var velocity = Vector()

    func initialize() {
        self.velocity.x = 1
        self.velocity.y = 2
    }

    func update() {
        position.add(velocity.x, velocity.y)
    }
}

var object = Object()
object.initialize()
object.update()
object.update()
object.update()
object.position.x + object.position.y
