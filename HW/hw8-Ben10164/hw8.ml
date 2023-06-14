(*
   Name: Ben Puryear
   File: hw8.ml
   Date: Spring 2023
   Desc: Implements the features required of HW8
*)

let my_min x y = if x > y then y else x

let my_median x y z =
  if (y <= x && x <= z) || (z <= x && x <= y) then x
  else if (x <= y && y <= z) || (z <= y && y <= y) then y
  else z

let my_triangle_area base height = base *. height /. 2.

let my_circle_area radius = 3.14 *. (radius ** 2.)

let my_midpoint (x1, y1) (x2, y2) = 
  ((x1 +. x2) /. 2., (y1 +. y2) /. 2.)

let my_manhattan_distance (x1, y1) (x2, y2) =
  abs_float (x1 -. x2) +. abs_float (y1 -. y2)

let my_euclidean_distance (x1, y1) (x2, y2) =
  sqrt (((x2 -. x1) ** 2.) +. ((y2 -. y1) ** 2.))

let rec my_range_sum v1 v2 =
  if v1 > v2 then 0 else v1 + my_range_sum (v1 + 1) v2

let rec my_gcd x y = 
  if x == 0 then y
  else my_gcd (y mod x) x

let rec my_even x = 
  if x < 0 then false
  else if x == 0 then true
  else my_odd (x-1)

and my_odd x =
  if x < 0 then false
  else if x == 1 then true
  else my_even(x-1)

let rec my_last xs =
  if List.length xs == 1 then List.hd xs
  else my_last (List.tl xs)

let rec my_rev xs = 
  if List.length xs == 1 then xs
  else List.append (my_rev(List.tl xs)) (List.hd xs :: [])

let rec my_init xs = 
  if List.length xs >= 1 then 
    if List.length xs == 2 then (List.hd xs)::[]
    else List.append(List.hd xs :: []) (my_init(List.tl xs))
  else failwith "Empty List"

let rec my_mem v xs =
  if List.length xs == 0 then false
  else 
    if List.hd xs == v then true
    else my_mem v (List.tl xs)

let rec my_replace (o, r) xs = 
  if List.length xs == 1 then
    if List.hd xs == o then (r::[]) 
    else xs
  else
    if List.hd xs == o then List.append (r::[]) (my_replace (o, r) (List.tl xs))
    else List.append ((List.hd xs) :: []) (my_replace (o, r) (List.tl xs))

let rec my_replace_all rs xs =
if List.length rs != 0 then
  let (o, r) = List.hd rs
  in  my_replace_all (List.tl rs) (my_replace (o, r) xs)
else xs

let rec my_elem_sum v xs = 
  if List.length xs == 0 then 0
  else 
    if List.hd xs == v then
      v + (my_elem_sum v (List.tl xs))
    else 0 + (my_elem_sum v (List.tl xs))

(* This one took the the longest because I wanted it to go from left to right... 
   But then I realized the test were not like that. Whoops *)
let rec my_rem_dups xs = 
  if List.length xs == 0 then []
  else if my_mem (List.hd xs) (List.tl xs) then my_rem_dups(List.tl xs)
  else (List.hd xs)::my_rem_dups(List.tl xs)

let rec my_list_min xs = 
  if List.length xs != 0 then 
    if List.length xs == 1 then List.hd xs
    else
      if (List.hd xs) <= (List.hd (List.tl xs)) then
        my_list_min ((List.hd xs) :: (List.tl (List.tl xs)))
      else my_list_min(List.tl xs)
else failwith "Empty List"
