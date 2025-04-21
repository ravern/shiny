class Foo {
    var x = 0
    var y = 0

    func bar(x: Int, y: Int) {
        self.x = x
        self.y = y
    }

    func baz() -> Int {
        return self.x + self.y
    }
}

var foo = Foo()
foo.bar(1, 2)
foo.baz()
