define i32 @main() {
    entry:
        %count = alloca i32
        store i32 0, i32* %count
        
        br label %loop
        
    loop:
        %current = load i32, i32* %count
        
        %cmp = icmp slt i32 %current, 10
        br i1 %cmp, label %body, label %exit
        
    body:
        %current_val = load i32, i32* %count

        %str = alloca [4 x i8]
        %format = alloca [3 x i8]
        store [3 x i8] c"%d\00", [3 x i8]* %format
        %str_result = call i32 @sprintf([4 x i8]* %str, [3 x i8]* %format, i32 %current_val)

        %put_result = call i32 @puts(i8* %str)
        
        %next = add i32 %current_val, 1
        store i32 %next, i32* %count

        br label %loop
        
    exit:
        ret i32 0
}

declare i32 @puts(i8*)
declare i32 @sprintf(i8*, [4 x i8]*, i32)
