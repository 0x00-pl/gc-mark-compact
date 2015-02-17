;;square
(define (square x)
    (* x x))

(display (square 21))(display " == 441")(newline)
(display (square (+ 2 5)))(display " == 49")(newline)
(display (square (square 3)))(display " == 81")(newline)



;;sum of squares
(define (sum-of-squares x y)
    (+ (square x) (square y)))
(display
(sum-of-squares 3 4)
)(display " == 25")(newline)

(define (f a)
(sum-of-squares (+ a 1) (* a 2)))
(display
(f 5)
)(display " == 136")(newline)

(define (check val result)
 (begin (display val) (display result) (newline)))

;;abs
(define (abs x)
(cond ((< x 0) (- x))
(else x)))

(check (abs 10) " == 10")
(check (abs -10) " == 10")

(define (abs-1 x)
(if (< x 0)
(- x) x))

(check (abs-1 10) " == 10")
(check (abs-1 -10) " == 10")

;;>=
(define (>= x y)
(or (> x y) (= x y)))

(display "test >= ")
(check (>= 1 2) " == false")
(check (>= 2 1) " == true")
(check (>= 2 2) " == true")

;;<=
(define (<= x y)
(or (< x y) (= x y)))

(display "test <= ")(newline)
(check (<= 1 234) " == true")
(check (<= 1 1) " == true")
(check (<= 0.2 1.0) " == true")

;;sqrt
(define (sqrt-iter guess x)
    (if (good-enough? guess x)
        guess
        (sqrt-iter  (improve guess x) x)))

(define (improve guess x)
    (average guess (/ x guess)))

(define (average x y)
    (/ (+ x y) 2))

(define (good-enough? guess x)
    (< (abs (- (square guess) x)) 0.001))
(define (sqrt x)
    (sqrt-iter 1 x))

(display "sqrt================")(newline)
(check (sqrt 9) " == 3")
(check (sqrt (+ 100 37)) " == 11.704699917758145")
;11.704699917758145
(check (sqrt (+ (sqrt 2) (sqrt 3))) " == 1.7739279023207892")
;1.7739279023207892
(check (square (sqrt 1000)) " == 1000")
;1000.000369924366

;;new-if
(define (new-if predicate then-clause else-clause)
(cond (predicate then-clause)
(else else-clause )))


(check (new-if (= 2 3) 0 5) " == 5")
;5
(check (new-if (= 1 1) 0 5) " == 0")
;0


;;;a bug please fix it.
(define (sqrt-iter guess x)
(if
(good-enough? guess x) guess
(sqrt-iter (improve guess x) x)))

;(load "lib.lisp")
;(trace 'average)
;(trace 'improve)
;(trace 'sqrt-iter)
;(trace 'good-enough?)
;(trace 'new-if)



;;factorial
(define (factorial n)
(if (= n 1)
1(* n (factorial (- n 1)))))

(check (factorial 6) " == 6! == 720")

;;fib
(define (fib n)
(cond ((= n 0) 0)
((= n 1) 1)
(else (+ (fib (- n 1))
(fib (- n 2))))))

(check (fib 6) " == fib(6) == 8")

;;gcd
(define (gcd a b)
(if (= b 0)
a(gcd b (remainder a b))))

(check (gcd 206 0) " == 206")
(check (gcd 1024 6) " == 2")

;;sum
(define (sum term a next b)
(if (> a b)
0(+ (term a)
(sum term (next a) next b))))

(define (cube x) (* x x x))
(define (inc n) (+ n 1))
(define (sum-cubes a b)
(sum cube a inc b))

(check (sum-cubes 1 10) " == 3025")
;3025





