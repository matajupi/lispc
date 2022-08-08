(define fib
    (lambda (n)
        (define iter
            (lambda (n a b)
                (if (= n 0)
                    a
                    (iter (- n 1) b (+ a b)))))
        (iter n 0 1)))

(print (int->string (fib 10)))
