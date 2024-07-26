
# interface

## init / fini funcitons

### cvector_init
```c
void cvector_init(void)
```
Initialize cvector data type. Should be called once before any other cvector
function.
> example
> ```c
> int main()
> {
> 	cvector_init();
> 	// some code using cvector
> 	cvector_fini();
> 	return 0;
> }
> ```


### cvector_fini
```c
void cvector_fini(void)
```
Finalize usage of cvector data type. Should be caleed once before exiting the
process.
> example
> ```c
> int main()
> {
> 	cvector_init();
> 	// some code using cvector
> 	cvector_fini();
> 	return 0;
> }
> ```



## create / destroy functions

### cvector_create
```c
type *cvector_create(type, uint64 size, cvector_create_flags_t flags)
```
| cvector_create_flags_t       | description |
|:---------------------------- |:------------|
| CVECTOR_CREATE_EXACT_SIZE    |             |
| CVECTOR_CREATE_ZERO          |             |
| CVECTOR_CREATE_ONLY_PREALLOC |             |


### cvector_destroy
```c
void cvector_destroy(type *vector)
```

### cvector_copy
```c
type *cvector_copy(type *vector)
```




--------------------------------------------------------------------------------
```c
// =============================================================================
void cvector_init(void); /* done */
void cvector_fini(void); /* done */

// =============================================================================
type *cvector_create(type, size, flags); /* done */
flags = {
	CVECTOR_CREATE_EXACT_SIZE
	CVECTOR_CREATE_ZERO
	CVECTOR_CREATE_ONLY_PREALLOC
};

void cvector_destroy(type *vector); /* done */

type *cvector_copy(const type *vector, flags) /* done */
flags = {
	CVECTOR_COPY_EXACT_SIZE
};

type *cvector_create_from(flags, type *begin, type *end) /* done */
flags = {
	CVECTOR_CREATE_FROM_EXACT_SIZE
};
type *cvector_create_from_list(type, flags, {initializer_list}) /* done */
flags = /* same from cvector_create_from */

// =============================================================================
type = cvector[index]; /* done */

type cvector_at(const type *vector, idx); /* done */
type cvector_rat(const type *vector, idx) /* done */
type cvector_front(const type *vector); /* done */
type cvector_back(const type *vector); /* done */

// =============================================================================
bool cvector_empty(const type *vector); /* done */
uint64 cvector_size(const type *vector); /* done */
uint64 cvector_capacity(const type *vector); /* done */

// =============================================================================
#define cvector_for_each(type *vector, type val) /* done */
#define cvector_for_each_reverse(type *cvector, type val) /* done */

type *cvector_find(const type *vector, type val, cmp); /* done */
type *cvector_rfind(const type *vector, type val, cmp); /* done */

bool cvector_contains(const type *vector, type val, cmp); /* done */
uint64 cvector_count(const type *vector, type val, cmp); /* done */

// =============================================================================
type *cvector_insert(type **vector, type *pos, type value, flags); /* done */
flags = {
	CVECTOR_INSERT_NO_EXPAND
	CVECTOR_INSERT_EXPAND_EXACT_SIZE
};
type *cvector_push_back(type **vector, type value, flags); /* done */
flags = /* same from cvector_insert */
type *cvector_push_front(type **vector, type value, flags); /* done */
flags = /* same from cvector_insert */

type *cvector_erase(type **vector, type *pos, flaga);
flags = {
	CVECTOR_ERASE_FORCE_SHRINK
	CVECTOR_ERASE_NO_SHRINK
};
type *cvector_pop_front(type **vector);
flags = /* same from cvector_erase */
type *cvector_pop_back(type **vector);
flags = /* same from cvector_erase */

int cvector_extend(type **vector, type *other, flags);
flags = {
	CVECTOR_EXTEND_EXACT_SIZE,
	CVECTOR_EXTEND_NO_EXPAND,
}

// =============================================================================
int cvector_shrink(type **vector);
int cvector_resize(type **vector, uint64 size, flags);
flags = {
	CVECTOR_RESIZE_EXACT_SIZE,
	CVECTOR_RESIZE_NO_EXPAND,
	CVECTOR_RESIZE_NO_SHRINK,
	CVECTOR_RESIZE_FORCE_SHRINK,
};
int cvector_reserve(type **vector, uint64 size, flags);
flags = {

};
int cvector_resize_ext(type **vectirm uint64 size, uint64 capacity);

// =============================================================================
type *cvector_begin(const type *vector); /* done */
type *cvector_end(const type *vector); /* done */
const type *cvector_cbegin(type *vector); /* skipped */
const type *cvector_cend(type *vector); /* skipped */

type *cvector_rbegin(const type *vector); /* begin */
type *cvector_rend(const type *vector); /* begin */
const type *cvector_crbegin(const type *vector); /* skipped */
const type *cvector_crend(const type *vector); /* skipped */

// =============================================================================
```