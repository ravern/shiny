func assignClosureTest() -> Int {
    var x = 1

    func closure() -> Int {
        return x
    }

    x = 2
    var y = 3
    var z = 10

    return closure() // returns 2, since `x` was captured by reference
}

assignClosureTest()