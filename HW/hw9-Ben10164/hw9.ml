(*
   Name: Ben Puryear
   File: hw9.ml
   Date: Spring 2023
   Desc: Implements all functions required in HW9
*)

(* 'a list -> 'a *)
let rec my_last xs =
  match xs with
  | [] -> failwith "Empty List"
  | [x] -> x
  | _ :: t -> my_last t
;;

(* 'a list -> 'a list *)
let rec my_init xs =
  match xs with
  | [] -> failwith "Empty List"
  | [x; y] -> [x]
  | x :: t -> [x] @ my_init t
;;

(* 'a * 'a -> 'a list -> 'a list *)
let rec my_replace (o, r) xs =
  match xs with
  | [] -> []
  | [x] when x = o -> [r]
  | [x] -> [x]
  | x :: t when x = o -> [r] @ my_replace (o, r) t
  | x :: t -> [x] @ my_replace (o, r) t
;;

(* ('a * 'a) list -> 'a list -> 'a list *)
let rec my_replace_all rs xs =
  match rs with
  | [] -> xs
  | x :: t -> my_replace_all t (my_replace x xs)
;;

(* int -> int list -> int *)
let rec my_elem_sum v xs =
  match xs with
  | [] -> 0
  | x :: t when x = v -> v + my_elem_sum v t
  | _ :: t -> my_elem_sum v t
;;

(* int -> int -> int *)
let rec my_range_sum v1 v2 =
  match v1 with
  | v when v > v2 -> 0
  | v -> v + my_range_sum (v1 + 1) v2
;;

(* Type Declaration *)
type ('a, 'b) kvlist =
  | Node of 'a * 'b * ('a, 'b) kvlist
  | Nil
;;

(* 'a -> 'b -> ('a, 'b) kvlist -> ('a, 'b) kvlist *)
let insert a b kvlist = Node (a, b, kvlist)

(* 'a -> ('a, 'b) kvlist -> ('a, 'b) kvlist *)
let rec remove a kvlist =
  match kvlist with
  | Nil -> Nil
  | Node (a', _, kvlist') when a' = a -> remove a kvlist'
  | Node (a', b', kvlist') -> Node (a', b', remove a kvlist')
;;

(* ('a, 'b) kvlist -> int *)
let rec size kvlist =
  match kvlist with
  | Nil -> 0
  | Node (_, _, kvlist') -> 1 + size kvlist'
;;

(* 'a -> ('a, 'b) kvlist -> 'b list *)
let rec has_key a kvlist =
  match kvlist with
  | Nil -> false
  | Node (a', _, _) when a' = a -> true
  | Node (_, _, kvlist') -> has_key a kvlist'
;;

(* 'a -> ('a, 'b) kvlist -> 'b list *)
let rec key_values a kvlist =
  match kvlist with
  | Nil -> []
  | Node (a', b', kvlist') when a' = a -> b' :: key_values a kvlist'
  | Node (a', _, kvlist') -> key_values a kvlist'
;;

(* ('a, 'b) kvlist -> ('a, 'b) kvlist -> ('a, 'b) kvlist *)
let rec combine kvlist1 kvlist2 =
  match kvlist1, kvlist2 with
  | Nil, kvlist2' -> kvlist2'
  | Node (a', b', kvlist1'), kvlist2' -> Node (a', b', combine kvlist1' kvlist2')
;;

(* ('a, 'b) kvlist -> ('a, 'b list) kvlist *)
let rec group kvlist =
  match kvlist with
  | Nil -> Nil
  | Node (a', b', kvlist') -> Node (a', b' :: key_values a' kvlist', group (remove a' kvlist'))
;;

(* ('a, 'b) kvlist -> ('b, 'a) kvlist *)
let rec invert kvlist =
  match kvlist with
  | Nil -> Nil
  | Node (a', b', c') -> Node (b', a', invert c')
;;

(* ('a -> 'b -> bool) -> ('a, 'b) kvlist -> ('a, 'b) kvlist *)
let rec kv_filter f kvlist =
  match kvlist with
  | Nil -> Nil
  | Node (a', b', kvlist') when f a' b' = true -> Node (a', b', kv_filter f kvlist')
  | Node (a', b', kvlist') -> kv_filter f kvlist'
;;

(* ('a -> 'b -> 'c * 'd) -> ('a, 'b) kvlist -> ('c, 'd) kvlist *)
let rec kv_map f kvlist =
  match kvlist with
  | Nil -> Nil
  | Node (a', b', kvlist') -> Node (fst (f a' b'), snd (f a' b'), kv_map f kvlist')
;;

(* int -> ('a, 'b) kvlist -> ('b, int) kvlist *)
let count_keys_by_val a kvlist = 
  kv_map (fun x y -> (x, (List.length y))) (kv_filter (fun x y -> (List.length y) > a ) (group (invert kvlist))) 
;;

