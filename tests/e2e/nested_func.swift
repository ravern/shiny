func makeMultiplier(x: Int) -> (Int) -> Int {
    func multiply(y: Int) -> Int {
        return x * y
    }
    return multiply
}

var multiplier = makeMultiplier(5)
multiplier(5)
