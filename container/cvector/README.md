
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




```c
// *****************************************************************************
// ****************************** BASIC INTERFACE ******************************
// *****************************************************************************

// -----------------------------------------------------------------------------
// macro
#define CVECTOR_CAPACITY_AUTO
#define CVECTOR_OUT_OF_RANGE

// -----------------------------------------------------------------------------
// external
extern void *cvectorAllocate(uint64 size); /* cvector external allocator */
extern void cvectorDeallocate(void *ptr); /* cvector external deallocator */
extern void cvectorPanic(int errorcode); /* cvector external exception handler */

// -----------------------------------------------------------------------------
// initialization
void cvectorInit(void);
void cvectorFini(void);

// -----------------------------------------------------------------------------
// construction
type *cvectorCreate(type, uint64 size, uint64 capacity, type fill);
/* capacity: number,
             CVECTOR_CAPACITY_AUTO: set capacity automatically
   return: pointer
           EINVAL: capacity < size
           ENOMEM: allocation failure
 */

void cvectorDestroy(type *vector);

type *cvectorCopy(const type *vector, uint64 capacity);
/* capacity: number
             CVECTOR_CAPACITY_AUTO: set copied vector capacity automatically
   return: pointer
           EINVAL: capacity < size
           ENOMEM: allocation failure
 */

// -----------------------------------------------------------------------------
// element access
type = cvector[index]; /* noexcept */

type cvectorAt(const type *vector, idx); /* throw CVECTOR_OUT_OF_RANGE */
type cvectorFront(const type *vector); /* throw CVECTOR_OUT_OF_RANGE */
type cvectorBack(const type *vector); /* throw CVECTOR_OUT_OF_RANGE */

// -----------------------------------------------------------------------------
// capacity
bool cvectorEmpty(const type *vector);
uint64 cvectorSize(const type *vector);
uint64 cvectorCapacity(const type *vector);

// -----------------------------------------------------------------------------
// modifiers
type *cvectorInsert(type *vector, type *pos, type value);
/* return: pointer to inserted entry
           ENOSPACE: no left capacity to insert new value
 */

type *cvectorPushBack(type *vector, type value);
/* return: pointer to inserted entry
           ENOSPACE: no left capacity to insert new value
 */

type *cvectorPushFront(type *vector, type value);
/* return: pointer to inserted entry
           ENOSPACE: no left capacity to insert new value
 */

void cvectorErase(type *vector, type *pos);
void cvectorPopBack(type *vector, type *pos);
void cvectorPopFront(type *vector, type *pos);

// -----------------------------------------------------------------------------
// resizing
int cvectorExpand(type **vector, uint64 capacity);
/* capacity: number: force reallocate vector to chosen capacity, size remains unchanged
             CVECTOR_CAPACITY_AUTO: check if vector has empty space to at least
               one extra entry, and if not - reallocates vector with automatically
               chosen capacity
   return: 0: ok
           EINVAL: capacity < current size
           ENOMEM: allocation failure
 */

int cvectorShink(type **vector);
/* return: 0: ok
           ENOMEM: allocation failure
 */

// -----------------------------------------------------------------------------
// iterators
type *cvectorBegin(const type *vector);
type *cvectorEnd(const type *vector);


// *****************************************************************************
// **************************** EXTENDED INTERFACE *****************************
// *****************************************************************************

// -----------------------------------------------------------------------------
// construction
type *cvectorCreateFrom(const type *begin, const type *end, uint64 capacity);
/* capacity: number
             CVECTOR_CAPACITY_AUTO: set created vector capacity automatically
   return: pointer
           EINVAL: capacity < (end - begin)
           ENOMEM: allocation failure
 */

type *cvectorCreateFromList(type, uint64 capacity, {initializer_list});
/* capacity: number
             CVECTOR_CAPACITY_AUTO: set created vector capacity automatically
   return: pointer
           EINVAL: capacity < ARRAY_SIZE(initializer_list)
           ENOMEM: allocation failure
*/

// -----------------------------------------------------------------------------
// element access
type cvectorRat(const type *vector, idx); /* throw CVECTOR_OUT_OF_RANGE */

// -----------------------------------------------------------------------------
// modifiers
type *cvectorInsertFrom(type *vector, type *pos, const type *begin, const type *end); /* maybe */
/* return: pointer to first inserted entry
           ENOSPACE: no left capacity to insert new values
 */

void cvectorEraseFrom(type *vector, type *start, type *end);

type *cvectorExtend(type *vector, type *other);
/* return: pointer to first inserted entry
           ENOSPACE: no left capacity to insert new values
 */

// -----------------------------------------------------------------------------
// resizing
int cvectorResize(type *vector, uint64 size, uint64 capacity);
/* size: new size of vector, if size > current size - values of new elements will
         be zeroed; if size < current size - extra elements will be remoevd
         from vector
   capacity: number: vector will be forced to reallocate to this capacity
             CVECTOR_CAPACITY_AUTO: set capacity automatically, if possible - do
             not reallocate vector
 */

// -----------------------------------------------------------------------------
// iterators
type *cvectorRbegin(const type *vector);
type *cvectorRend(const type *vector);

// -----------------------------------------------------------------------------
// traverse
typedef int (*cmp_t)(type a, type b); /* uses values, not pointers */

#define cvectorForEach(type *vector, type val) {} /* for loop */
#define cvectorForEachRev(type *cvector, type val) {} /* reverse for loop */

// -----------------------------------------------------------------------------
// lookups
type *cvectorFind(const type *vector, type val, cmp_t cmp);
/* return: pointer to first occurrence of @val
           NULL: if val not foud
 */

type *cvectorRfind(const type *vector, type val, cmp_t cmp);
/* return: pointer to last occurrence of @val
           NULL: if val not found
 */

bool cvectorContains(const type *vector, type val, cmp_t cmp);
uint64 cvectorCount(const type *vector, type val, cmp_t cmp);

int cvectorCompare(const type *v1, const type *v2, cmp_t cmp);
/* return 0: if vectors @v1 and @v2 are equal
          cmp(v1[i], v2[i]): if cmp(v1[i], v2[i]) != 0
 */

bool cvectorEqual(const type *v1, const type *v2, cmp_t cmp);
/* return: true: if vector @v1 and @v2 are equal
           false: otherwise
 */

uint64 cvectorRemoveIf(type **vector, cmp_t cmp);
/* return: number of removed elements */


// *****************************************************************************
// ******************************* FUTURE DRAFT ********************************
// *****************************************************************************

// -----------------------------------------------------------------------------
// construction
int cvectorAssign(type **vector, type *begin, type *end);
int cvectorClear(type **vector, flags);

// -----------------------------------------------------------------------------
// iterators
const type *cvectorCbegin(type *vector);
const type *cvectorCend(type *vector);

const type *cvectorCRbegin(const type *vector);
const type *cvectorCRend(const type *vector);

void cvectorReverse(type *vector); /* maybe */
void cvectorSort(type *vector); /* maybe */

// -----------------------------------------------------------------------------
// lookups
type *cvectorFindIn(const type *begin, const type *end, type val, cmp_t cmp); /* maybe */
type *cvectorRfindIn(const type *begin, const type *end, type type val, cmp_t cmp); /* maybe */

```
