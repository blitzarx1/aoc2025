#include <string.h>

#ifdef LOG_NAME
#undef LOG_NAME
#endif
#define LOG_NAME "lib"
#include "log.h"

#define HASHMAP_IMPLEMENTATION
#include <hashmap.h>

enum Mode {
    MODE_MAP,
    MODE_FIND,
};

typedef struct {long long start, end;} Range;

typedef struct {
    long long sum;
    Array *ranges;
    enum Mode mode;
} Ctx;

static Range merge_ranges(Range r1, Range r2) {
    Range merged;

    merged.start = r1.start < r2.start ? r1.start : r2.start;
    merged.end = r1.end > r2.end ? r1.end : r2.end;

    LOG_INFO("Merged ranges: [%lld-%lld] + [%lld-%lld] --> [%lld-%lld]",
        r1.start, r1.end, r2.start, r2.end, merged.start, merged.end);

    return merged;
}

static Range parse_range(const char *line) {
    char *start = strtok((char *)line, "-");
    char *end = strtok(NULL, "-");

    return (Range){atoll(start), atoll(end)};
}

static int handle_range(const char *line, Ctx *ctx) {
    Range new_range = parse_range(line);

    bool inserted = false;
     for (int i=0; i < arrlen(ctx->ranges); i++) {
        Range *existing_range = ARR_GET(Range, ctx->ranges, i);

        if ((new_range.start <= existing_range->end) &&
            (new_range.end >= existing_range->start)) {
            Range merged = merge_ranges(*existing_range, new_range);
            if (!arrpop(ctx->ranges, i)) {
                LOG_ERROR("arrpop failed");
                return -1;
            }

            LOG_DEBUG("Removed existing range at idx %d; new arrlen: %zu", i, arrlen(ctx->ranges));
            new_range = merged;

            LOG_DEBUG("Updated new_range to merged; restarting check for further merges");
            i=-1; // restart to check for further merges
            continue;
        } else if (new_range.end < existing_range->start) {
            if (arrinsert(ctx->ranges, i, &new_range) != 0) {
                LOG_ERROR("arrinsert failed");
                return -1;
            }

            LOG_INFO("Inserted new range [%lld-%lld] before existing [%lld-%lld]", new_range.start, new_range.end, existing_range->start, existing_range->end);

            inserted = true;
            break;
        }
     }

     if (!inserted) {
        if (arrappend(ctx->ranges, &new_range) != 0) {
            LOG_ERROR("arrappend failed");
            return -1;
        }

        LOG_INFO("Appended range to the end: [%lld-%lld]", new_range.start, new_range.end);
     }

    return 0;
}

static bool contains(const char *numstr, Array *ranges) {
    long long num = atoll(numstr);

    for (int i=0; i < arrlen(ranges); i++) {
        Range *r = ARR_GET(Range, ranges, i);
        if (num >= r->start &&  num <= r->end) {
            LOG_INFO("Number %lld is within range [%lld-%lld]", num, r->start, r->end);
            return true;
        }
    }

    return false;
}

static long long compute_ranges(Array *ranges) {
    long long total = 0;

    LOG_INFO("Computing all ranges...");
    
    for (int i=0; i < arrlen(ranges); i++) {
        Range *r = ARR_GET(Range, ranges, i);
        LOG_INFO("Range [%lld-%lld]", r->start, r->end);
    }

    for (int i=0; i < arrlen(ranges); i++) {
        Range *r = ARR_GET(Range, ranges, i);
        total += (r->end - r->start + 1);
    }

    return total;
}

int process_lines(const char *line, void *ctx){
    Ctx *context = (Ctx *)ctx;

    if (strlen(line) == 0) {
        LOG_INFO("Empty line encountered mode changed");

#ifdef PART2
        long long total = compute_ranges(context->ranges);
        context->sum = total;
        return 1;
    }
#else
        context->mode = MODE_FIND;
        return 0;
    }
#endif

    switch (context->mode) {
        case MODE_MAP: 
            if (handle_range(line, context) != 0) {
                LOG_ERROR("handle_range failed");
                return -1;
            }
            break;
        case MODE_FIND:
            if (contains(line, context->ranges)) {
                context->sum++;
            }
            break;
        default:
            LOG_ERROR("unknown mode: %d", context->mode);
            return -1;
    }

    return 0;
}
