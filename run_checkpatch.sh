#!/bin/bash

INIT_COMMIT=$(git log --reverse --pretty=format:"%h" | head -n 1)
IGNORES=FILE_PATH_CHANGES,SPDX_LICENSE_TAG,MISSING_EOF_NEWLINE

HW6_IGNORES=$IGNORES,EXPORT_SYMBOL,ENOSYS,AVOID_EXTERNS,LINE_CONTINUATIONS,AVOID_BUG
git diff $INIT_COMMIT | linux/scripts/checkpatch.pl --ignore $HW6_IGNORES

# ENOSYS: Looks like ENOSYS is often misused in the kernel so checkpatch always
#         warns on seeing ENOSYS. Our use is correct, though!
#         (Nonexistent syscall was called)
# EXPORT_SYMBOL: Buggy for our use-case, let's just ignore this.
# AVOID_EXTERNS: Checkpatch will complain if you extern in a .c file instead of
#                a .h file.
# MISSING_EOF_NEWLINE: Expects a file to end with a newline, which isn't the
#                      case for symlinks.
# LINE_CONTINUATIONS: Sometimes these are useful, as is the case with long
#                     macros.
# AVOID_BUG: BUGs are essentially assertion statements that crash the kernel
#            if a condition doesn't hold. Sometimes we need to modify existing
#            BUGs.
