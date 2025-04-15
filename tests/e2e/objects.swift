class Foo {
    var x = 3

    func bar(y: Int) -> Int {
        return self.x + y
    }
}

var foo = Foo()
foo.bar(5)
