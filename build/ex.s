; ModuleID = 'ex_op.ll'
source_filename = "IsaLLVM"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@format_str = private unnamed_addr constant [4 x i8] c"%i\0A\00", align 1

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #0

; Function Attrs: nofree nounwind
define i32 @main(i32 %argc, ptr nocapture readnone %argv) local_unnamed_addr #0 {
entry:
  br label %body

body:                                             ; preds = %entry, %body
  %i.0 = phi i32 [ 0, %entry ], [ %addtmp3, %body ]
  %addtmp3 = add nuw nsw i32 %i.0, 1
  %0 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @format_str, i32 %addtmp3)
  %exitcond.not = icmp eq i32 %addtmp3, 1000000
  br i1 %exitcond.not, label %after_loop, label %body

after_loop:                                       ; preds = %body
  ret i32 0
}

attributes #0 = { nofree nounwind }
