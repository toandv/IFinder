
(or (>= p1_available 1)
    (>= p2_available 1)
    (>= p3_available 1)
    (>= p4_available 1)
    (>= p5_available 1)
    
    (and (>= (+ p5_interm p1_abst) 2) (>= p1_x 2))
    (and (>= (+ p4_interm p5_abst) 2) (>= p5_x 2))
    (and (>= (+ p3_interm p4_abst) 2) (>= p4_x 2))
    (and (>= (+ p2_interm p3_abst) 2) (>= p3_x 2))
    (and (>= (+ p1_interm p2_abst) 2) (>= p2_x 2))
    
    (and (=> (>= p1_available 1) (< p1_x 2))
         (=> (>= p2_available 1) (< p2_x 2))
         (=> (>= p3_available 1) (< p3_x 2))
         (=> (>= p4_available 1) (< p4_x 2))
         (=> (>= p5_available 1) (< p5_x 2))
    )
)
