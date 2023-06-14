(*
   Name: Ben Puryear
   File: hw9_tests.ml
   Date: Spring 2023
   Desc: Unit tests for HW9
*)

open Hw9

let msg = "--- Running HW9 Tests --- ";;
print_endline msg;;

(* equality assertions *)
let assert_equal v1 v2 msg =
  let cond = v1 = v2 in
  assert (if not cond then print_endline ("TEST FAILED: " ^ msg) ; cond)
;;


(* Question 1: my_last *)
assert_equal 'b' (my_last ['a'; 'y'; 'b']) "'b' = my_last ['a'; 'y'; 'b']";;
assert_equal 5 (my_last [1; 2; 3; 4; 5])"5 = my_last [1; 2; 3; 4; 5]";;
assert_equal 5. (my_last [1.; 2.; 3.; 4.; 5.])"5 = my_last [1; 2; 3; 4; 5]";;


(* Question 2: my_init *)
assert_equal [1; 2] (my_init [1; 2; 3]) "[1; 2] = my_init [1; 2; 3]";;
assert_equal [1; 2; 3] (my_init [1; 2; 3; 4]) "[1; 2; 3] = my_init [1; 2; 3; 4]";;
assert_equal [1; 2; 3; 4] (my_init [1; 2; 3; 4; 100]) "[1; 2; 3] = my_init [1; 2; 3; 4]";;

(* Question 3: my_rplace *)
assert_equal [1; 8; 3; 8] (my_replace (2, 8) [1; 2; 3; 2]) "[1; 8; 3; 8] = my_replace (2, 8) [1; 2; 3; 2]";;
assert_equal [1; 8; 3; 9] (my_replace (2, 8) [1; 2; 3; 9])"[1; 8; 3; 9] = my_replace (2, 8) [1; 2; 3; 9]";;
assert_equal [1; 2; 1; 2] (my_replace (3, 1) [1; 2; 3; 2]) "[1; 2; 1; 9] = my_replace (3, 1) [1; 2; 3; 2]";;

(* Question 4: my_replace_all *)
assert_equal ['b'; 'b'; 'd'; 'd'] (my_replace_all [('a', 'b'); ('c', 'd')] ['a'; 'b'; 'c'; 'd']) "['b'; 'b'; 'd'; 'd'] = my_replace_all [('a', 'b'); ('c', 'd')] ['a'; 'b'; 'c'; 'd']" ;;
assert_equal [3; 3; 3; 4] (my_replace_all [(1, 2); (2, 3)] [1; 2; 3; 4]) "[3; 3; 3; 4] = my_replace_all [(1, 2); (2, 3)] [1; 2; 3; 4]";;
assert_equal [100; 100; 3; 4] (my_replace_all [(1, 2); (2, 100)] [1; 2; 3; 4]) "[100; 100; 3; 4] = my_replace_all [(1, 2); (2, 100)] [1; 2; 3; 4]";;

(* Question 5: my_elem_sum *)
assert_equal 10 (my_elem_sum 10 [15; 10; 25]) "10 = my_elem_sum 10 [15; 10; 25]";;
assert_equal 12 (my_elem_sum 3 [3; 2; 3; 2; 3; 4; 3]) "12 = my_elem_sum 3 [3; 2; 3; 2; 3; 4; 3]";;
assert_equal 0 (my_elem_sum 3 []) "0 = my_elem_sum 3 []";;

(* Question 6: my_range_sum *)
assert_equal 2 (my_range_sum 2 2) "2 = my_range_sum 2 2";;
assert_equal 9 (my_range_sum 2 4) "9 = my_range_sum 2 4";;
assert_equal 0 (my_range_sum 2 1) "0 = my_range_sum 2 1";;

(* Question 7: insert *)
assert_equal (Node ('a', 1,  Nil)) (insert 'a' 1 Nil) "kvlist ('a', 1, Nil) = insert 'a' 1 Nil" ;;
assert_equal (Node('a', 1, Node('b', 1, Nil))) (insert 'a' 1  (Node('b', 1, Nil))) "insert 'a' 1 (Node ('b', 1, Nil)) = Node ('a', 1, Node ('b', 1, Nil))";;



(* Question 8: remove *)
assert_equal Nil (remove 'a' Nil) "remove 'a' Nil = Nil";;
assert_equal Nil (remove 'a' (Node('a', 1, Nil))) "remove 'a' (Node ('a', 1, Nil)) = Nil";;
assert_equal Nil (remove 'a' (Node('a', 1, Node('a', 2, Nil)))) "remove 'a' (Node ('a', 1, Node ('a', 2, Nil))) = Nil";;
assert_equal (Node('b', 1, Nil)) (remove 'a' (Node('b', 1, Node('a', 2, Nil)))) "remove 'a' (Node ('b', 1, Node ('a', 2, Nil))) = Nil";;

(* Question 9: size *)
assert_equal 0 (size Nil) "0 = size Nil";;
assert_equal 1 (size (Node('a', 1, Nil))) "1 = size (Node('a', 1, Nil))";;
assert_equal 2 (size (Node('a', 1, Node('a', 2, Nil)))) "1 = size (Node('a', 1, Node('a', 2, Nil)))";;

(* Question 10: has_key *)
assert_equal true (has_key 'b' (Node('b', 1, Nil))) "true = (has_key 'b' (Node('b', 1, Nil)))";;
assert_equal false (has_key 'c' (Node('b', 1, Nil)))"false = (has_key 'c' (Node('b', 1, Nil)))";;
assert_equal true (has_key 'b' (Node('a', 1, Node('b', 2, Nil))))"true = (has_key 'b' (Node('a', 1, Node('b', 2, Nil))))";;


(* Question 11: key_values *)
assert_equal [] (key_values 'a' Nil) "[] = key_values 'a' Nil";;
assert_equal [1] (key_values 'a' (Node ('a', 1, Node ('b', 2, Nil)))) "[1] = (key_values 'a' (Node ('a', 1, Node ('b', 2, Nil))))";;
assert_equal [2; 3] (key_values 'a' (Node ('a', 2, Node ('a', 3, Nil)))) "[2; 3] = (key_values 'a' (Node ('a', 2, Node ('a', 3, Nil))))";;
assert_equal [] (key_values 'c' (Node ('a', 1, Node ('b', 2, Nil))))"[] = (key_values 'c' (Node ('a', 1, Node ('b', 2, Nil))))";;

(* Question 12: combine *)
assert_equal (Node('a', 1, Node('b', 2, Nil))) (combine (Node('a', 1, Nil)) (Node('b', 2, Nil))) "(Node('a', 1, Node('b', 2, Nil))) = (combine (Node('a', 1, Nil)) (Node('b', 2, Nil)))";;
assert_equal (Node('a', 1, Node('b', 2, Node('c', 3, Nil)))) (combine (Node('a', 1, Nil)) (Node('b', 2, Node('c', 3, Nil)))) "(Node('a', 1, Node('b', 2, Node('c', 3, Nil)))) = (combine (Node('a', 1, Nil)) (Node('b', 2, Node('c', 3, Nil))))";;
assert_equal (Node('a', 1, Node('b', 2, Node('c', 3, Node('d', 4, Nil))))) (combine (Node('a', 1, Nil)) (Node('b', 2, Node('c', 3, Node('d', 4, Nil)))))"(Node('a', 1, Node('b', 2, Node('c', 3, Node('d', 4, Nil))))) = (combine (Node('a', 1, Nil)) (Node('b', 2, Node('c', 3, Node('d', 4, Nil)))))";;

(* Question 13: group *)
assert_equal Nil (group Nil)"Nil = (group Nil)";;
assert_equal (Node ('a', [1], Nil)) (group (Node ('a', 1, Nil))) "(Node ('a', [1], Nil)) = (group (Node ('a', 1, Nil)))";;
assert_equal (Node ('a', [1], Node ('b', [2], Nil))) (group (Node ('a', 1, Node ('b', 2, Nil)))) "(Node ('a', [1], Node ('b', [2], Nil))) = (group (Node ('a', 1, Node ('b', 2, Nil))))";;
assert_equal (Node ('a', [1; 2], Nil)) (group (Node ('a', 1, Node ('a', 2, Nil))))"(Node ('a', [1; 2], Nil)) = (group (Node ('a', 1, Node ('a', 2, Nil))))";;

(* Question 14: invert *)
assert_equal Nil (invert Nil) "Nil = (invert Nil)";;
assert_equal (Node (1, 'a', Nil)) (invert (Node ('a', 1, Nil))) "(Node (1, 'a', Nil)) = (invert (Node ('a', 1, Nil)))";;
assert_equal (Node (1, 'a', Node (2, 'b', Nil))) (invert (Node ('a', 1, Node ('b', 2, Nil)))) "(Node (1, 'a', Node (2, 'b', Nil))) = (invert (Node ('a', 1, Node ('b', 2, Nil))))";; 

(* Question 15: kv_filter *)
let vowel a b = 
  match a with
  | 'a' -> true
  | 'e' -> true
  | 'i' -> true
  | 'o' -> true
  | 'u' -> true
  | _ -> false
;;

assert_equal Nil (kv_filter vowel (Node('b', 2, Nil))) "Nil = (kv_filter vowel (Node('b', 2, Nil)))";;
assert_equal (Node('a', 2, Nil)) (kv_filter vowel (Node('a', 2, Node('b', 2, Nil)))) "(Node('a', 2, Nil)) = (kv_filter vowel (Node('a', 2, Node('b', 2, Nil))))";;
assert_equal (Node('a', 1, Node('a', 2, Nil))) (kv_filter vowel (Node('a', 1, Node('a', 2, Node('c', 3, Nil))))) "(Node('a', 1, Node('a', 2, Nil))) = (kv_filter vowel (Node('a', 1, Node('b', 2, Node('c', 3, Nil)))))";;

(* Question 16: kv_map *)
let vowel_map a b = 
  match (a,b) with
  | 'a', b' -> (true, b')
  | 'e', b' -> (true, b')
  | 'i', b' -> (true, b')
  | 'o', b' -> (true, b')
  | 'u', b' -> (true, b')
  | _, b' -> (false, b')
;;

assert_equal Nil (kv_map vowel_map Nil) "Nil = (kv_map vowel_map Nil))";;
assert_equal (Node(true, 5, Nil)) (kv_map vowel_map (Node('a', 5, Nil))) "(Node(true, 5, Nil)) = (kv_map vowel_map (Node('a', 5, Nil))))";;
assert_equal (Node(false, 5, Nil)) (kv_map vowel_map (Node('b', 5, Nil))) "(Node(false, 5, Nil)) = (kv_map vowel_map (Node('b', 5, Nil))))";;
assert_equal (Node(false, 5, (Node(true, 10, Nil)))) (kv_map vowel_map (Node('b', 5, (Node('o', 10, Nil))))) "(Node(false, 5, (Node(true, 10, Nil)))) = (kv_map vowel_map (Node('b', 5, (Node('o', 10, Nil)))))";;

(* Question 17: count_keys_by_val *)
assert_equal Nil (count_keys_by_val 0 Nil) "Nil = (count_keys_by_val 0 Nil)";;
assert_equal (Node (1, 1, Nil)) (count_keys_by_val 0 (Node ('a', 1, Nil)))"(Node (1, 1, Nil)) = (count_keys_by_val 0 (Node ('a', 1, Nil)))";;
assert_equal Nil (count_keys_by_val 1 (Node ('a', 1, Nil))) "Nil = (count_keys_by_val 1 (Node ('a', 1, Nil)))";;
assert_equal (Node (1, 2, Nil)) (count_keys_by_val 1 (Node ('a', 1, Node ('b', 1, Nil)))) "(Node (1, 2, Nil)) = (count_keys_by_val 1 (Node ('a', 1, Node ('b', 1, Nil))))";;
assert_equal Nil (count_keys_by_val 1 (Node ('a', 1, Node ('b', 2, Nil)))) "Nil = (count_keys_by_val 1 (Node ('a', 1, Node ('b', 2, Nil))))";; 
