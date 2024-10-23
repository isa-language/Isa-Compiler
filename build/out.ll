; ModuleID = 'IsaLLVM'
source_filename = "IsaLLVM"

declare i32 @printf(ptr, ...)
@format_str = private unnamed_addr constant [4 x i8] c"%i\0A\00", align 1
define i32 @main(i32 %argc, ptr %argv) {
entry:
  %num = alloca i32, align 4
  store i32 3000000, ptr %num, align 4
  %sum = alloca i32, align 4
  store i32 0, ptr %sum, align 4
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  br label %loop

loop:                                             ; preds = %body, %entry
  %load_left = load i32, ptr %i, align 4
  %cmp_lt = icmp slt i32 %load_left, 3000000
  br i1 %cmp_lt, label %body, label %after_loop

body:                                             ; preds = %loop
  %load_left1 = load i32, ptr %sum, align 4
  %load_right = load i32, ptr %i, align 4
  %addtmp = add i32 %load_left1, %load_right
  %load_left2 = load i32, ptr %i, align 4
  %current_value = load i32, ptr %i, align 4
  %updated_value = add i32 %current_value, 1
  store i32 %updated_value, ptr %i, align 4

  %fmt_ptr = bitcast [5 x i8]* @format_str to ptr
  %i_val = load i32, ptr %i, align 4
  call i32 (ptr, ...) @printf(ptr %fmt_ptr, i32 %i_val)
  br label %loop

after_loop:                                       ; preds = %loop
  ret i32 0 
}
