var cool = 1

class Foo {
    var integer = 0
    var double = 1.0
    var boolean = false

    func getInteger() -> Int {
        return self.integer
    }

    func getDouble() -> Double {
        return self.double
    }
}

var f = Foo()
var x = f.getDouble()

