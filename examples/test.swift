var x = 1
var y = 2
func add() -> ((Int) -> Int) {
    func add2(x: Int) -> Int {
        return x + 2
    }
    return add2
}
add()(2)