func fib(n: Int) -> Int {
    if n <= 1 {
        return n
    }
    return fib(n - 1) + fib(n - 2)
}

fib(10)