; ModuleID = 'hello.c'
source_filename = "hello.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @func() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 0, i32* %2, align 4
  br label %4

4:                                                ; preds = %19, %0
  %5 = load i32, i32* %2, align 4
  %6 = icmp slt i32 %5, 10
  br i1 %6, label %7, label %22

7:                                                ; preds = %4
  store i32 0, i32* %3, align 4
  br label %8

8:                                                ; preds = %15, %7
  %9 = load i32, i32* %3, align 4
  %10 = icmp slt i32 %9, 10
  br i1 %10, label %11, label %18

11:                                               ; preds = %8
  %12 = load i32, i32* %2, align 4
  %13 = load i32, i32* %1, align 4
  %14 = add nsw i32 %13, %12
  store i32 %14, i32* %1, align 4
  br label %15

15:                                               ; preds = %11
  %16 = load i32, i32* %3, align 4
  %17 = add nsw i32 %16, 1
  store i32 %17, i32* %3, align 4
  br label %8, !llvm.loop !4

18:                                               ; preds = %8
  br label %19

19:                                               ; preds = %18
  %20 = load i32, i32* %2, align 4
  %21 = add nsw i32 %20, 1
  store i32 %21, i32* %2, align 4
  br label %4, !llvm.loop !6

22:                                               ; preds = %4
  %23 = load i32, i32* %1, align 4
  ret i32 %23
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @func2(i32 %0, i32 %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i32 %1, i32* %4, align 4
  %6 = call i32 @func()
  store i32 %6, i32* %5, align 4
  %7 = load i32, i32* %5, align 4
  %8 = load i32, i32* %3, align 4
  %9 = mul nsw i32 %7, %8
  %10 = load i32, i32* %4, align 4
  %11 = mul nsw i32 %9, %10
  ret i32 %11
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @getResult(i32 %0, i32 %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 %0, i32* %4, align 4
  store i32 %1, i32* %5, align 4
  store i32 0, i32* %6, align 4
  %7 = call i32 @func()
  %8 = icmp slt i32 %7, 10
  br i1 %8, label %9, label %13

9:                                                ; preds = %2
  %10 = load i32, i32* %4, align 4
  %11 = load i32, i32* %5, align 4
  %12 = call i32 @func2(i32 %10, i32 %11)
  store i32 %12, i32* %6, align 4
  br label %15

13:                                               ; preds = %2
  %14 = call i32 @func2(i32 0, i32 0)
  store i32 %14, i32* %6, align 4
  br label %15

15:                                               ; preds = %13, %9
  %16 = load i32, i32* %3, align 4
  ret i32 %16
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"uwtable", i32 1}
!2 = !{i32 7, !"frame-pointer", i32 2}
!3 = !{!"clang version 13.0.1"}
!4 = distinct !{!4, !5}
!5 = !{!"llvm.loop.mustprogress"}
!6 = distinct !{!6, !5}
