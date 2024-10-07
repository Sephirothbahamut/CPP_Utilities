#pragma once

#include "../return_types.h"
#include "../../bezier.h"
#include "../../../../vec4.h"

namespace utils::math::geometry::sdf::details::bezier::_4pt
	{
	namespace free_functions
		{
		using ::operator*; //TODO understand why `side * float` in `return_types` namespace hides `vec<float, extent> * float` that's declared in global namespace

		//All from: https://www.shadertoy.com/view/4sKyzW
		//TODO try replace 1e38f with float infinity

		/*
		Exact distance to cubic bezier curve by computing roots of the derivative(s)
		to isolate roots of a fifth degree polynomial and Halley's Method to compute them.
		Inspired by https://www.shadertoy.com/view/4sXyDr and https://www.shadertoy.com/view/ldXXWH
		See also my approximate version:
		https://www.shadertoy.com/view/lsByRG
		*/
		const float              eps     { .000005f};
		const float              zoom    {1.f      };
		const float              dot_size{ .005f};
		const utils::math::vec3f point_col{1.f, 1.f, 0.f};
		const int halley_iterations{8};

		//lagrange positive real root upper bound
		//see for example: https://doi.org/10.1016/j.jsc.2014.09.038
		inline float upper_bound_lagrange5(float a0, float a1, float a2, float a3, float a4) 
			{
			utils::math::vec4f coeffs1 = utils::math::vec4f(a0, a1, a2, a3);

			utils::math::vec4f neg1 = utils::math::max(-coeffs1, utils::math::vec4f{0.f});
			float neg2 = utils::math::max(-a4, 0.f);

			const utils::math::vec4f indizes1 = utils::math::vec4f(0.f, 1.f, 2.f, 3.f);
			const float indizes2 = 4.f;

			utils::math::vec4f bounds1 = utils::math::pow(neg1, utils::math::vec4f{1.f} / (-indizes1 + 5.f));
			float bounds2 = std::pow(neg2, 1.f / (5.f - indizes2));

			utils::math::vec2f min1_2 = utils::math::min(bounds1.xz(), bounds1.yw());
			utils::math::vec2f max1_2 = utils::math::max(bounds1.xz(), bounds1.yw());

			float maxmin = utils::math::max(min1_2.x(), min1_2.y());
			float minmax = utils::math::min(max1_2.x(), max1_2.y());

			float max3 = utils::math::max(max1_2.x(), max1_2.y());

			float max_max = utils::math::max(max3, bounds2);
			float max_max2 = utils::math::max(utils::math::min(max3, bounds2), utils::math::max(minmax, maxmin));

			return max_max + max_max2;
			}

		//lagrange upper bound applied to f(-x) to get lower bound
		inline float lower_bound_lagrange5(float a0, float a1, float a2, float a3, float a4)
			{
			//See comments of the shadertoy page suggesting to replace the commented function bodyy with this line.
			const auto ret{-1.0f * upper_bound_lagrange5(-a0, a1, -a2, a3, -a4)};
			return ret;

			//utils::math::vec4f coeffs1 = utils::math::vec4f(-a0, a1, -a2, a3);
			//
			//utils::math::vec4f neg1 = utils::math::max(-coeffs1, utils::math::vec4f(0));
			//float neg2 = utils::math::max(-a4, 0.f);
			//
			//const utils::math::vec4f indizes1 = utils::math::vec4f(0, 1, 2, 3);
			//const float indizes2 = 4.f;
			//
			//utils::math::vec4f bounds1 = utils::math::pow(neg1, utils::math::vec4f{1.f} / (-indizes1 + 5.f));
			//float bounds2 = std::pow(neg2, 1.f / (5.f - indizes2));
			//
			//utils::math::vec2f min1_2 = utils::math::min(bounds1.xz(), bounds1.yw());
			//utils::math::vec2f max1_2 = utils::math::max(bounds1.xz(), bounds1.yw());
			//
			//float maxmin = utils::math::max(min1_2.x(), min1_2.y());
			//float minmax = utils::math::min(max1_2.x(), max1_2.y());
			//
			//float max3 = utils::math::max(max1_2.x(), max1_2.y());
			//
			//float max_max = utils::math::max(max3, bounds2);
			//float max_max2 = utils::math::max(utils::math::min(max3, bounds2), utils::math::max(minmax, maxmin));
			//
			//return -max_max - max_max2;
			}

		inline utils::math::vec2f parametric_cub_bezier(float t, utils::math::vec2f p0, utils::math::vec2f p1, utils::math::vec2f p2, utils::math::vec2f p3)
			{
			utils::math::vec2f a0 = (-p0 + p1 * 3.f - p2 * 3.f + p3);
			utils::math::vec2f a1 = (p0 * 3.f - p1 * 6.f + p2 * 3.f);
			utils::math::vec2f a2 = (p0 * -3.f + p1 * 3.f);
			utils::math::vec2f a3 = p0;

			return (((a0 * t) + a1) * t + a2) * t + a3;
			}

		inline void sort_roots3(utils::math::vec3f& roots)
			{
			utils::math::vec3f tmp;

			tmp[0] = utils::math::min(roots[0], utils::math::min(roots[1], roots[2]));
			tmp[1] = utils::math::max(roots[0], utils::math::min(roots[1], roots[2]));
			tmp[2] = utils::math::max(roots[0], utils::math::max(roots[1], roots[2]));

			roots = tmp;
			}

		inline void sort_roots4(utils::math::vec4f& roots)
			{
			utils::math::vec4f tmp;

			utils::math::vec2f min1_2 = utils::math::min(roots.xz(), roots.yw());
			utils::math::vec2f max1_2 = utils::math::max(roots.xz(), roots.yw());

			float maxmin = utils::math::max(min1_2.x(), min1_2.y());
			float minmax = utils::math::min(max1_2.x(), max1_2.y());

			tmp[0] = utils::math::min(min1_2.x(), min1_2.y());
			tmp[1] = utils::math::min(maxmin, minmax);
			tmp[2] = utils::math::max(minmax, maxmin);
			tmp[3] = utils::math::max(max1_2.x(), max1_2.y());

			roots = tmp;
			}

		inline float eval_poly5(float a0, float a1, float a2, float a3, float a4, float x)
			{
			const float f = ((((x + a4) * x + a3) * x + a2) * x + a1) * x + a0;

			return f;
			}

		//halley's method
			//basically a variant of newton raphson which converges quicker and has bigger basins of convergence
			//see http://mathworld.wolfram.com/HalleysMethod.html
			//or https://en.wikipedia.org/wiki/Halley%27s_method
		inline float halley_iteration5(float a0, float a1, float a2, float a3, float a4, float x)
			{
			float f = ((((x + a4) * x + a3) * x + a2) * x + a1) * x + a0;
			float f1 = (((5.f * x + 4.f * a4) * x + 3.f * a3) * x + 2.f * a2) * x + a1;
			float f2 = ((20.f * x + 12.f * a4) * x + 6.f * a3) * x + 2.f * a2;

			return x - (2.f * f * f1) / (2.f * f1 * f1 - f * f2);
			}

		inline float halley_iteration4(utils::math::vec4f coeffs, float x)
			{
			float f = (((x + coeffs[3]) * x + coeffs[2]) * x + coeffs[1]) * x + coeffs[0];
			float f1 = ((4.f * x + 3.f * coeffs[3]) * x + 2.f * coeffs[2]) * x + coeffs[1];
			float f2 = (12.f * x + 6.f * coeffs[3]) * x + 2.f * coeffs[2];

			return x - (2.f * f * f1) / (2.f * f1 * f1 - f * f2);
			}

		// Modified from http://tog.acm.org/resources/GraphicsGems/gems/Roots3And4.c
		// Credits to Doublefresh for hinting there
		inline int solve_quadric(utils::math::vec2f coeffs, utils::math::vecref2f roots)
			{
			// normal form: x^2 + px + q = 0
			float p = coeffs[1] / 2.f;
			float q = coeffs[0];

			float D = p * p - q;

			if (D < 0.f) 
				{
				return 0;
				}
			else if (D > 0.f) 
				{
				roots[0] = -sqrt(D) - p;
				roots[1] = sqrt(D) - p;

				return 2;
				}
			}

		//From Trisomie21
		//But instead of his cancellation fix i'm using a newton iteration
		inline int solve_cubic(utils::math::vec3f coeffs, utils::math::vecref3f r)
			{
			float a = coeffs[2];
			float b = coeffs[1];
			float c = coeffs[0];

			float p = b - a * a / 3.0f;
			float q = a * (2.0f * a * a - 9.0f * b) / 27.0f + c;
			float p3 = p * p * p;
			float d = q * q + 4.0f * p3 / 27.0f;
			float offset = -a / 3.0f;
			if (d >= 0.0f) { // Single solution
				float z = sqrt(d);
				float u = (-q + z) / 2.0f;
				float v = (-q - z) / 2.0f;
				u = sign(u) * std::pow(utils::math::abs(u), 1.0f / 3.0f);
				v = sign(v) * std::pow(utils::math::abs(v), 1.0f / 3.0f);
				r[0] = offset + u + v;

				//Single newton iteration to account for cancellation
				float f = ((r[0] + a) * r[0] + b) * r[0] + c;
				float f1 = (3.f * r[0] + 2.f * a) * r[0] + b;

				r[0] -= f / f1;

				return 1;
				}
			float u = sqrt(-p / 3.0f);
			float v = acos(-sqrt(-27.0f / p3) * q / 2.0f) / 3.0f;
			float m = cos(v), n = sin(v) * 1.732050808;

			//Single newton iteration to account for cancellation
			//(once for every root)
			r[0] = offset + u * (m + m);
			r[1] = offset - u * (n + m);
			r[2] = offset + u * (n - m);

			utils::math::vec3f f = ((r + a) * r + b) * r + c;
			utils::math::vec3f f1 = (r * 3.f + 2.f * a) * r + b;

			r -= f / f1;

			return 3;
			}

		// Modified from http://tog.acm.org/resources/GraphicsGems/gems/Roots3And4.c
		// Credits to Doublefresh for hinting there
		inline int solve_quartic(utils::math::vec4f coeffs, utils::math::vecref4f s)
			{
			float a = coeffs[3];
			float b = coeffs[2];
			float c = coeffs[1];
			float d = coeffs[0];

			/*  substitute x = y - A/4 to eliminate cubic term:
			x^4 + px^2 + qx + r = 0 */

			float sq_a = a * a;
			float p = -3.f / 8.f * sq_a + b;
			float q = 1.f / 8.f * sq_a * a - 1.f / 2.f * a * b + c;
			float r = -3.f / 256.f * sq_a * sq_a + 1.f / 16.f * sq_a * b - 1.f / 4.f * a * c + d;

			int num;

			/* doesn't seem to happen for me */
			//if(abs(r)<eps){
			//	/* no absolute term: y(y^3 + py + q) = 0 */

			//	utils::math::vec3f cubic_coeffs;

			//	cubic_coeffs[0] = q;
			//	cubic_coeffs[1] = p;
			//	cubic_coeffs[2] = 0.f;

			//	num = solve_cubic(cubic_coeffs, s.xyz);

			//	s[num] = 0.f;
			//	num++;
			//} else
			if (true)
				{
				/* solve the resolvent cubic ...f */

				utils::math::vec3f cubic_coeffs;

				cubic_coeffs[0] = 1.0f / 2.f * r * p - 1.0f / 8.f * q * q;
				cubic_coeffs[1] = -r;
				cubic_coeffs[2] = -1.0f / 2.f * p;

				solve_cubic(cubic_coeffs, s.xyz());

				/* ...f and take the one real solution ...f */

				float z = s[0];

				/* ...f to build two quadric equations */

				float u = z * z - r;
				float v = 2.f * z - p;

				if (u > -eps) 
					{
					u = sqrt(abs(u));
					}
				else { return 0; }

				if (v > -eps) 
					{
					v = sqrt(abs(v));
					}
				else { return 0; }

				utils::math::vec2f quad_coeffs;

				quad_coeffs[0] = z - u;
				quad_coeffs[1] = q < 0.f ? -v : v;

				num = solve_quadric(quad_coeffs, s.xy());

				quad_coeffs[0] = z + u;
				quad_coeffs[1] = q < 0.f ? v : -v;

				utils::math::vec2f tmp{1e38f};
				int old_num = num;

				num += solve_quadric(quad_coeffs, tmp);
				if (old_num != num) {
					if (old_num == 0) {
						s[0] = tmp[0];
						s[1] = tmp[1];
						}
					else {//old_num == 2
						s[2] = tmp[0];
						s[3] = tmp[1];
						}
					}
				}

			/* resubstitute */

			float sub = 1.f / 4.f * a;

			/* single halley iteration to fix cancellation */
			for (int i = 0; i < 4; i += 2) 
				{
				if (i < num) 
					{
					s[i] -= sub;
					s[i] = halley_iteration4(coeffs, s[i]);

					s[i + 1] -= sub;
					s[i + 1] = halley_iteration4(coeffs, s[i + 1]);
					}
				}

			return num;
			}

		//Sign computation is pretty straightforward:
		//I'm solving a cubic equation to get the intersection count
		//of a ray from the current point to infinity and parallel to the x axis
		//Also i'm computing the intersection count with the tangent in the end points of the curve
			inline float cubic_bezier_sign(utils::math::vec2f uv, utils::math::vec2f p0, utils::math::vec2f p1, utils::math::vec2f p2, utils::math::vec2f p3)
			{
			float cu = (-p0.y() + 3.f * p1.y() - 3.f * p2.y() + p3.y());
			float qu = (3.f * p0.y() - 6.f * p1.y() + 3.f * p2.y());
			float li = (-3.f * p0.y() + 3.f * p1.y());
			float co = p0.y() - uv.y();

			utils::math::vec3f roots = utils::math::vec3f(1e38f);
			int n_roots = solve_cubic(utils::math::vec3f(co / cu, li / cu, qu / cu), roots);

			int n_ints = 0;

			for (int i = 0; i < 3; i++) 
				{
				if (i < n_roots) 
					{
					if (roots[i] >= 0.f && roots[i] <= 1.f) 
						{
						float x_pos = -p0.x() + 3.f * p1.x() - 3.f * p2.x() + p3.x();
						x_pos = x_pos * roots[i] + 3.f * p0.x() - 6.f * p1.x() + 3.f * p2.x();
						x_pos = x_pos * roots[i] + -3.f * p0.x() + 3.f * p1.x();
						x_pos = x_pos * roots[i] + p0.x();

						if (x_pos < uv.x()) {
							n_ints++;
							}
						}
					}
				}

			utils::math::vec2f tang1 = p0.xy() - p1.xy();
			utils::math::vec2f tang2 = p2.xy() - p3.xy();

			utils::math::vec2f nor1 = utils::math::vec2f(tang1.y(), -tang1.x());
			utils::math::vec2f nor2 = utils::math::vec2f(tang2.y(), -tang2.x());

			if (p0.y() < p1.y()) 
				{
				if ((uv.y() <= p0.y()) && (utils::math::vec2f::dot(uv - p0.xy(), nor1) < 0.f)) 
					{
					n_ints++;
					}
				}
			else
				{
				if (!(uv.y() <= p0.y()) && !(utils::math::vec2f::dot(uv - p0.xy(), nor1) < 0.f)) 
					{
					n_ints++;
					}
				}

			if (p2.y() < p3.y()) 
				{
				if (!(uv.y() <= p3.y()) && utils::math::vec2f::dot(uv - p3.xy(), nor2) < 0.f) 
					{
					n_ints++;
					}
				}
			else 
				{
				if ((uv.y() <= p3.y()) && !(utils::math::vec2f::dot(uv - p3.xy(), nor2) < 0.f)) 
					{
					n_ints++;
					}
				}

			if (n_ints == 0 || n_ints == 2 || n_ints == 4) 
				{
				return 1.f;
				}
			else 
				{
				return -1.f;
				}
			}

		struct distance_closest_and_t
			{
			float distance;
			utils::math::vec2f closest;
			float t;
			};

		inline distance_closest_and_t cubic_bezier_dis(utils::math::vec2f uv, utils::math::vec2f p0, utils::math::vec2f p1, utils::math::vec2f p2, utils::math::vec2f p3)
			{
			//switch points when near to end point to minimize numerical error
			//only needed when control point(s) very far away
			#if 0
			utils::math::vec2f mid_curve = parametric_cub_bezier(.5, p0, p1, p2, p3);
			utils::math::vec2f mid_points = (p0 + p3) / 2.f;

			utils::math::vec2f tang = mid_curve - mid_points;
			utils::math::vec2f nor = utils::math::vec2f(tang.y(), -tang.x());

			if (sign(dot(nor, uv - mid_curve)) != sign(dot(nor, p0 - mid_curve))) {
				utils::math::vec2f tmp = p0;
				p0 = p3;
				p3 = tmp;

				tmp = p2;
				p2 = p1;
				p1 = tmp;
				}
			#endif

			utils::math::vec2f a3 = (-p0 + p1 * 3.f - p2 * 3.f + p3);
			utils::math::vec2f a2 = (p0 * 3.f - p1 * 6.f + p2 * 3.f);
			utils::math::vec2f a1 = (p0 * -3.f + p1 * 3.f);
			utils::math::vec2f a0 = p0 - uv;

			//compute polynomial describing distance to current pixel dependent on a parameter t
			float bc6 = utils::math::vec2f::dot(a3, a3);
			float bc5 = 2.f * utils::math::vec2f::dot(a3, a2);
			float bc4 = utils::math::vec2f::dot(a2, a2) + 2.f * utils::math::vec2f::dot(a1, a3);
			float bc3 = 2.f * (utils::math::vec2f::dot(a1, a2) + utils::math::vec2f::dot(a0, a3));
			float bc2 = utils::math::vec2f::dot(a1, a1) + 2.f * utils::math::vec2f::dot(a0, a2);
			float bc1 = 2.f * utils::math::vec2f::dot(a0, a1);
			float bc0 = utils::math::vec2f::dot(a0, a0);

			bc5 /= bc6;
			bc4 /= bc6;
			bc3 /= bc6;
			bc2 /= bc6;
			bc1 /= bc6;
			bc0 /= bc6;

			//compute derivatives of this polynomial

			float b0 = bc1 / 6.f;
			float b1 = 2.f * bc2 / 6.f;
			float b2 = 3.f * bc3 / 6.f;
			float b3 = 4.f * bc4 / 6.f;
			float b4 = 5.f * bc5 / 6.f;

			utils::math::vec4f c1 = utils::math::vec4f(b1, 2.f * b2, 3.f * b3, 4.f * b4) / 5.f;
			utils::math::vec3f c2 = utils::math::vec3f(c1[1], 2.f * c1[2], 3.f * c1[3]) / 4.f;
			utils::math::vec2f c3 = utils::math::vec2f(c2[1], 2.f * c2[2]) / 3.f;
			float c4 = c3[1] / 2.f;

			utils::math::vec4f roots_drv = utils::math::vec4f(1e38f);

			int num_roots_drv = solve_quartic(c1, roots_drv);
			sort_roots4(roots_drv);

			float ub = upper_bound_lagrange5(b0, b1, b2, b3, b4);
			float lb = lower_bound_lagrange5(b0, b1, b2, b3, b4);

			utils::math::vec3f a = utils::math::vec3f(1e38f);
			utils::math::vec3f b = utils::math::vec3f(1e38f);

			utils::math::vec3f roots = utils::math::vec3f(1e38f);

			int num_roots = 0;

			//compute root isolating intervals by roots of derivative and outer root bounds
			//only roots going form - to + considered, because only those result in a minimum
			if (num_roots_drv == 4) 
				{
				if (eval_poly5(b0, b1, b2, b3, b4, roots_drv[0]) > 0.f)
					{
					a[0] = lb;
					b[0] = roots_drv[0];
					num_roots = 1;
					}

				if (sign(eval_poly5(b0, b1, b2, b3, b4, roots_drv[1])) != sign(eval_poly5(b0, b1, b2, b3, b4, roots_drv[2])))
					{
					if (num_roots == 0) 
						{
						a[0] = roots_drv[1];
						b[0] = roots_drv[2];
						num_roots = 1;
						}
					else
						{
						a[1] = roots_drv[1];
						b[1] = roots_drv[2];
						num_roots = 2;
						}
					}

				if (eval_poly5(b0, b1, b2, b3, b4, roots_drv[3]) < 0.f)
					{
					if (num_roots == 0) 
						{
						a[0] = roots_drv[3];
						b[0] = ub;
						num_roots = 1;
						}
					else if (num_roots == 1)
						{
						a[1] = roots_drv[3];
						b[1] = ub;
						num_roots = 2;
						}
					else
						{
						a[2] = roots_drv[3];
						b[2] = ub;
						num_roots = 3;
						}
					}
				}
			else 
				{
				if (num_roots_drv == 2) 
					{
					if (eval_poly5(b0, b1, b2, b3, b4, roots_drv[0]) < 0.f) 
						{
						num_roots = 1;
						a[0] = roots_drv[1];
						b[0] = ub;
						}
					else if (eval_poly5(b0, b1, b2, b3, b4, roots_drv[1]) > 0.f)
						{
						num_roots = 1;
						a[0] = lb;
						b[0] = roots_drv[0];
						}
					else 
						{
						num_roots = 2;

						a[0] = lb;
						b[0] = roots_drv[0];

						a[1] = roots_drv[1];
						b[1] = ub;
						}

					}
				else //num_roots_drv==0
					{
					utils::math::vec3f roots_snd_drv = utils::math::vec3f(1e38f);
					int num_roots_snd_drv = solve_cubic(c2, roots_snd_drv);

					utils::math::vec2f roots_trd_drv = utils::math::vec2f(1e38f);
					int num_roots_trd_drv = solve_quadric(c3, roots_trd_drv);
					num_roots = 1;

					a[0] = lb;
					b[0] = ub;
					}

				//further subdivide intervals to guarantee convergence of halley's method
				//by using roots of further derivatives
				utils::math::vec3f roots_snd_drv = utils::math::vec3f(1e38f);
				int num_roots_snd_drv = solve_cubic(c2, roots_snd_drv);
				sort_roots3(roots_snd_drv);

				int num_roots_trd_drv = 0;
				utils::math::vec2f roots_trd_drv = utils::math::vec2f(1e38f);

				if (num_roots_snd_drv != 3) 
					{
					num_roots_trd_drv = solve_quadric(c3, roots_trd_drv);
					}

				for (int i = 0; i < 3; i++)
					{
					if (i < num_roots) 
						{
						for (int j = 0; j < 3; j += 2)
							{
							if (j < num_roots_snd_drv)
								{
								if (a[i] < roots_snd_drv[j] && b[i] > roots_snd_drv[j])
									{
									if (eval_poly5(b0, b1, b2, b3, b4, roots_snd_drv[j]) > 0.f)
										{
										b[i] = roots_snd_drv[j];
										}
									else 
										{
										a[i] = roots_snd_drv[j];
										}
									}
								}
							}

						for (int j = 0; j < 2; j++) 
							{
							if (j < num_roots_trd_drv) 
								{
								if (a[i] < roots_trd_drv[j] && b[i] > roots_trd_drv[j])
									{
									if (eval_poly5(b0, b1, b2, b3, b4, roots_trd_drv[j]) > 0.f) 
										{
										b[i] = roots_trd_drv[j];
										}
									else 
										{
										a[i] = roots_trd_drv[j];
										}
									}
								}
							}
						}
					}
				}

			float d0 = 1e38f;


			if (num_roots == 0)
				{//TODO remove, Just checking if this ever happens
				std::cout << "my check tmp" << std::endl;
				}
			assert(num_roots > 0);

			float found_t{std::numeric_limits<float>::quiet_NaN()};
			utils::math::vec2f found_closest;

			//compute roots with halley's method
			for (int i = 0; i < 3; i++)
				{
				if (i < num_roots) 
					{
					roots[i] = .5 * (a[i] + b[i]);

					for (int j = 0; j < halley_iterations; j++)
						{
						roots[i] = halley_iteration5(b0, b1, b2, b3, b4, roots[i]);
						}


					//compute squared distance to nearest point on curve
					roots[i] = utils::math::clamp(roots[i], 0.f, 1.f);
					utils::math::vec2f to_curve = uv - parametric_cub_bezier(roots[i], p0, p1, p2, p3);

					const auto candidate_distance2{utils::math::vec2f::dot(to_curve, to_curve)};
					if (candidate_distance2 < d0)
						{
						d0 = candidate_distance2;
						found_t = roots[i];
						found_closest = to_curve;
						}
					}
				}

			const auto distance{std::sqrt(d0)};

			return distance_closest_and_t{.distance{distance}, .closest{found_closest}, .t{found_t}};
			}
		}

	template<storage::type storage_type, geometry::ends::optional_ab optional_ends>
	struct sdf_proxy
		{
		using shape_t = geometry::shape::generic::bezier<storage_type, 4, optional_ends>;

		sdf_proxy(const shape_t& shape, const vec2f& point) : shape{shape}, point{point} {};
		const shape_t& shape;
		const vec2f point;

		template <ends::ab ends>
		utils_gpu_available constexpr float closest_t() const noexcept
			{
			const auto stuff{free_functions::cubic_bezier_dis(point, shape.vertices[0], shape.vertices[1], shape.vertices[2], shape.vertices[3])};
			return stuff.t;
			}
		};
	}