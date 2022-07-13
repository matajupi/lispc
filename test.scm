(define (fib a)
    (if (< a 2)
        a
        (+ (fib (- a 1)) (fib (- a 2)))))

(define (make-pair a b)
    (lambda (m)
        (cond ((string= m "get-car") a)
              ((string= m "get-cdr") b)
              ((string= m "set-car!") (lambda (new-a) (set! a new-a)))
              ((string= m "set-cdr!") (lambda (new-b) (set! b new-b)))
              (else (error "UNKNOWN-MESSAGE" m)))))

(define (cons a b) (make-pair a b))
(define (car p) (p "get-car"))
(define (cdr p) (p "get-cdr"))
(define (set-car! p new-a) ((p "set-car!") new-a))
(define (set-cdr! p new-b) ((p "set-cdr!") new-b))

