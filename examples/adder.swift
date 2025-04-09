func makeAdder(x: Int) -> ((Int) -> Int) {
    func adder(y: Int) -> Int {
        return x + y
    }
    return adder
}

var oneAdder = makeAdder(1)
var result = oneAdder(2)
result