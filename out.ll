; ModuleID = 'IsaLLVM'
source_filename = "IsaLLVM"

@format = private constant [4 x i8] c"%d\0A\00"

declare i32 @printf(ptr, i32)

define i32 @main(i32 %argc, ptr %argv) {
entry:
  %format = alloca ptr, align 8
  store ptr @format, ptr %format, align 8
  %num = alloca i32, align 4
  store i32 3000000, ptr %num, align 4
  %sum = alloca i32, align 4
  store i32 0, ptr %sum, align 4
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  br label %loop

loop:                                             ; preds = %body, %entry
  %load_left = load i32, ptr %i, align 4
  %current_value = load i32, ptr %i, align 4
  %updated_value = add i32 %current_value, 1
  store i32 %updated_value, ptr %i, align 4
  %load_left1 = load i32, ptr %i, align 4
  %cmp_le = icmp sle i32 %load_left1, 3000000
  br i1 %cmp_le, label %body, label %after_loop

body:                                             ; preds = %loop
  %calltmp = call i32 @printf(ptr %format, ptr %i)
  %load_left2 = load i32, ptr %sum, align 4
  %load_right = load i32, ptr %i, align 4
  %current_value3 = load i32, ptr %sum, align 4
  %updated_value4 = add i32 %current_value3, %load_right
  store i32 %updated_value4, ptr %sum, align 4
  %fmt_ptr = bitcast [5 x i8]* @format_str to ptr
  %i_val = load i32, ptr %i, align 4
  br label %loop

after_loop:                                       ; preds = %loop
  ret i32 0
}
