(define make-pair
    (lambda (v1 v2)
        (lambda (m)
            (if (string= m "get-car") v1
            (if (string= m "get-cdr") v2
            (if (string= m "set-car!") (lambda (new-v1) (set! v1 new-v1))
            (if (string= m "set-cdr!") (lambda (new-v2) (set! v2 new-v2)))))))))

(define cons (lambda (v1 v2) (make-pair v1 v2)))
(define car (lambda (p) (p "get-car")))
(define cdr (lambda (p) (p "get-cdr")))
(define set-car! (lambda (p new-v) ((p "set-car!") new-v)))
(define set-cdr! (lambda (p new-v) ((p "set-cdr!") new-v)))

;(define print-pair
;    (lambda (p)
;        (print "car:")
;        (print (car p))
;        (print "cdr:")
;        (print (cdr p))))
;
;(define pair (cons "carだよ" "cdrだよ"))
;(print-pair pair)
;(set-car! pair "再代入したcarだよ")
;(set-cdr! pair "再代入したcdrだよ")
;(print-pair pair)

(define print-lst
    (lambda (lst m)
        (define iter
            (lambda (lst)
                (if (null? lst)
                    "ok"
                    (begin
                        (if (string= m "int")
                            (print (int->string (car lst)))
                            (print (car lst)))
                        (iter (cdr lst))))))
        (iter lst)))

(define lst null)
(define create-lst
    (lambda (n)
        (if (= n 0)
            null
            (cons n (create-lst (- n 1))))))

(define lst (create-lst 10))
(print-lst lst "int")

