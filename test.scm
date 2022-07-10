(define (pow a b)
    (if (= b 0)
        1
        (* (pow a (- b 1)) a)))
