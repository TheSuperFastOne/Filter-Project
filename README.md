If you've ever seen one of those filter videos:

https://www.tiktok.com/@janieceandisaiah/video/7486545694417816874

Like one of these for example. This is essentially the inspiration for the project. This simulation conserves energy perfectly with fully elastic collisions.
You can use statistical models to get a distribution for how many bounces on average happen. For large numbers of bounces it approximately follows a bernoulli distribution.
For small bounces (b) I'm thinking of trying to use a Markov Chain in the future but for now I want to get data on the geometric distribution statistic (p). This probability density function should be of the form ((1-p)^(b-1))*p.
This geometric distribution is the distribution to find the chance of the first success of a given number of independent trials. Obviously, where one ball bounces is not independent of the next. A bounce far from the hole in the middle should in theory be less likely to result in a success (falling through)
than a bounce right in the middle. The idea is that the chance of each state starts to settle into predictable probabilities, so the independence still isn't there but so heavily shuffled that it works for larger b, like b>7 or so.
