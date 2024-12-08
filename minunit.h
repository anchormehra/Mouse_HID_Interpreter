/**
 * \ingroup TestSuite
 * \{
 * \addtogroup minunit
 * \{
 * \brief Minimalist unit testing framework.
 */
// Found at: http://www.jera.com/techinfo/jtns/jtn002.html
#ifndef MINUNIT_H
#define MINUNIT_H
#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; if (message) return message; } while (0)
extern int tests_run;
extern int mu_verbosity;
#define mu_print(val,verb) if(mu_verbosity>=verb){printf val ;}

#endif //MINUNIT_H
/**
 * \}\}
 */