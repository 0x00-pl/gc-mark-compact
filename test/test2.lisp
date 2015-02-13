(define x '(a b c d e f))
(display (car x))(newline)
(display (cdr x))(newline)

(define x '(g g g))
(set! x '(a b c d e f))
(display x)(newline)

(define x1 '(((a b c d e f))))
(display x1)(newline)
(display (cdr x1))(newline)