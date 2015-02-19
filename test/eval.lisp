(while 1 
 (begin 
  (eval 
   (parser
    (read);"((display (+ 1 1)))"
   )
  )
 )
)