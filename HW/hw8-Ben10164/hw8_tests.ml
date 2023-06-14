(*
   Name: Ben Puryear
   File: hw8_tests.ml
   Date: Spring 2023
   Desc: Unit tests for HW8
*)

open Hw8

let msg = "--- Running HW8 Tests --- ";;
print_endline msg;;

(* equality assertions *)
let assert_equal v1 v2 msg =
  let cond = v1 = v2 in
  assert (if not cond then print_endline ("TEST FAILED: " ^ msg) ; cond)
;;

(* Question 1: my_min tests *)
assert_equal 1 (my_min 1 2) "1 = my_min 1 2";;
assert_equal 1 (my_min 2 1) "1 = my_min 2 1";;
assert_equal 1 (my_min 1 1) "1 = my_min 1 1";;

(* Question 2: my_median tests *)
assert_equal 2 (my_median 1 2 3) "2 = my_median 1 2 3";;
assert_equal 2 (my_median 3 2 1) "2 = my_median 3 2 1";;
assert_equal 2 (my_median 2 2 10) "2 = my_median 2 2 2";;

(* Question 3: my_triangle_area *)
assert_equal 35.0 (my_triangle_area 10. 7.) "35.0 = my_triangle_area 10. 7.";;
assert_equal 55.0 (my_triangle_area 11. 10.) "55.0 = my_triangle_area 11. 10.";;
assert_equal 0.5 (my_triangle_area 1. 1.) "0.5 = my_triangle_area 1. 1.";;

(* Question 4: my_circle_area *)
assert_equal 3.14 (my_circle_area 1.) "3.14 = my_circle_area 1.";;
assert_equal 314. (my_circle_area 10.) "314. = my_circle_area 10.";;
assert_equal 78.5 (my_circle_area 5.) "78.5 = my_circle_area 5";;

(* Question 5: my_midpoint *)
assert_equal (2., 3.) (my_midpoint (1., 1.) (3., 5.)) "(2., 3.) = my_midpoint (1., 1.) (3., 5.)";;
assert_equal (3., 1.) (my_midpoint (3., 1.) (3., 1.)) "(3., 1.) = my_midpoint (3., 1.) (3., 1.)";;
assert_equal (5., 5.) (my_midpoint (0., 0.) (10., 10.)) "(5., 5.) = my_midpoint (0., 0.) (10., 10.)";;

(* Question 6: my_manhattan_distance *)
assert_equal 5. (my_manhattan_distance (1., 1.) (3., 4.)) "5. = my_manhattan_distance (1., 1.) (3., 4.)";;
assert_equal 5. (my_manhattan_distance (3., 1.) (1., 4.)) "5. = my_manhattan_distance (3., 1.) (1., 4.)";;
assert_equal 0. (my_manhattan_distance (1., 1.) (1., 1.)) "0. = my_manhattan_distance (1., 1.) (1., 1.)";;

(* Question 7: my_euclidean_distance *)
assert_equal 5. (my_euclidean_distance (1., 1.) (4., 5.)) "5. = my_euclidean_distance (1., 1.) (4., 5.)";;
assert_equal 4. (my_euclidean_distance (1., 6.) (1., 2.)) "4. = my_euclidean_distance (1., 6.) (1., 2.)";;
assert_equal 0. (my_euclidean_distance (1., 6.) (1., 6.)) "0. = my_euclidean_distance (1., 6.) (1., 6.)";;

(* Question 8: my_range_sum *)
assert_equal 2 (my_range_sum 2 2) "2 = my_range_sum 2 2";;
assert_equal 9 (my_range_sum 2 4) "9 = my_range_sum 2 4";;
assert_equal 0 (my_range_sum 2 1) "0 = my_range_sum 2 1";;

(* Question 9: my_gcd *)
assert_equal 12 (my_gcd 24 36) "12 = my_gcd 24 36";;
assert_equal 6 (my_gcd 18 42) "6 = my_gcd 18 42";;
assert_equal 2 (my_gcd 24 34) "2 = my_gcd 24 34";;

(* Question 10a: my_even *)
assert_equal true (my_even 4) "true = my_even 4";;
assert_equal true (my_even 110) "true = my_even 110";;
assert_equal false (my_even 111) "false = my_even 111";;

(* Question 10b: my_odd *)
assert_equal false (my_odd 4) "false = my_odd 4";;
assert_equal false (my_odd 110) "false = my_odd 110";;
assert_equal true (my_odd 111) "true = my_odd 111";;

(* Question 11: my_last *)
assert_equal 'b' (my_last ['a'; 'y'; 'b']) "'b' = my_last ['a'; 'y'; 'b']";;
assert_equal 5 (my_last [1; 2; 3; 4; 5])"5 = my_last [1; 2; 3; 4; 5]";;
assert_equal 5. (my_last [1.; 2.; 3.; 4.; 5.])"5 = my_last [1; 2; 3; 4; 5]";;

(* Question 12: my_rev *)
assert_equal [3; 2; 1] (my_rev [1; 2; 3]) "[3; 2; 1] = my_rev [1; 2; 3]";;
assert_equal [1; 100; 3] (my_rev [3; 100; 1]) "[1; 100; 3] = my_rev [3; 100; 1]";;
assert_equal [1; 100; 3; 10; 2] (my_rev [2; 10; 3; 100; 1]) "[1; 100; 3; 10; 2] = my_rev [2; 10; 3; 100; 1]";;

(* Question 13: my_init *)
assert_equal [1; 2] (my_init [1; 2; 3]) "[1; 2] = my_init [1; 2; 3]";;
assert_equal [1; 2; 3] (my_init [1; 2; 3; 4]) "[1; 2; 3] = my_init [1; 2; 3; 4]";;
assert_equal [1; 2; 3; 4] (my_init [1; 2; 3; 4; 100]) "[1; 2; 3] = my_init [1; 2; 3; 4]";;

(* Question 14: my_mem *)
assert_equal true (my_mem 5 [1; 3; 5; 7]) "true = my_mem 5 [1; 3; 5; 7]";;
assert_equal false (my_mem 5 [1; 3; 6; 7]) "false = my_mem 5 [1; 3; 6; 7]";;
assert_equal false (my_mem 5 []) "false = my_mem 5 []";;

(* Question 15: my_rplace *)
assert_equal [1; 8; 3; 8] (my_replace (2, 8) [1; 2; 3; 2]) "[1; 8; 3; 8] = my_replace (2, 8) [1; 2; 3; 2]";;
assert_equal [1; 8; 3; 9] (my_replace (2, 8) [1; 2; 3; 9])"[1; 8; 3; 9] = my_replace (2, 8) [1; 2; 3; 9]";;
assert_equal [1; 2; 1; 2] (my_replace (3, 1) [1; 2; 3; 2]) "[1; 2; 1; 9] = my_replace (3, 1) [1; 2; 3; 2]";;

(* Question 16: my_replace_all *)
assert_equal ['b'; 'b'; 'd'; 'd'] (my_replace_all [('a', 'b'); ('c', 'd')] ['a'; 'b'; 'c'; 'd']) "['b'; 'b'; 'd'; 'd'] = my_replace_all [('a', 'b'); ('c', 'd')] ['a'; 'b'; 'c'; 'd']" ;;
assert_equal [3; 3; 3; 4] (my_replace_all [(1, 2); (2, 3)] [1; 2; 3; 4]) "[3; 3; 3; 4] = my_replace_all [(1, 2); (2, 3)] [1; 2; 3; 4]";;
assert_equal [100; 100; 3; 4] (my_replace_all [(1, 2); (2, 100)] [1; 2; 3; 4]) "[100; 100; 3; 4] = my_replace_all [(1, 2); (2, 100)] [1; 2; 3; 4]";;

(* Question 17: my_elem_sum *)
assert_equal 10 (my_elem_sum 10 [15; 10; 25]) "10 = my_elem_sum 10 [15; 10; 25]";;
assert_equal 12 (my_elem_sum 3 [3; 2; 3; 2; 3; 4; 3]) "12 = my_elem_sum 3 [3; 2; 3; 2; 3; 4; 3]";;
assert_equal 0 (my_elem_sum 3 []) "0 = my_elem_sum 3 []";;

(* Question 18: my_rem_dups *)
assert_equal ['c'; 'b'; 'a'] (my_rem_dups ['a'; 'b'; 'a'; 'c'; 'b'; 'a']) "['c'; 'b'; 'a'] = my_rem_dups ['a'; 'b'; 'a'; 'c'; 'b'; 'a']";;
assert_equal [10; 11; 13; 11; 12] (my_rem_dups [10; 13; 11; 12]);;
assert_equal ['a'; 'b'] (my_rem_dups ['a'; 'b']) "['a'; 'b'] = my_rem_dups ['a'; 'b']";;

(* Question 19: my_list_min *)
assert_equal 1 (my_list_min [7; 1; 9; 12; 10]) "1 = my_list_min [7; 1; 9; 12; 10]";;
assert_equal 0 (my_list_min [7; 1; 9; 12; 0]) "0 = my_list_min [7; 1; 9; 12; 0]";;
assert_equal 10 (my_list_min [70; 10; 90; 120; 80]) "0 = my_list_min [7; 1; 9; 12; 0]";;
