#include <cmath>

#include "../../../../vec.h"

#include "4pt.h"
#include "../return_types.h"
#include "../../bezier.h"
#include "../../../../vec4.h"

namespace utils::math::geometry::sdf::details::bezier::_4pt::free_functions
	{
	using ::operator*; //TODO understand why `side * float` in `return_types` namespace hides `vec<float, extent> * float` that's declared in global namespace

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Slower but comprehensible
	// https://www.shadertoy.com/view/sdjXRy
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// https://www.shadertoy.com/view/st33Wj
	utils_gpu_available constexpr utils::math::vec2f cmul(utils::math::vec2f z, utils::math::vec2f w) noexcept
		{
		//return mat2(z, -z.y(), z.x()) * w; 
		// [z.x(), -z.y()] * [w.x()] = [z.x() * w.x() - z.y() * w.y()]
		// [z.y(),  z.x()]   [w.y()]   [z.y() * w.x() + z.x() * w.y()]
		return
			{
			z.x() * w.x() - z.y() * w.y(), 
			z.y() * w.x() + z.x() * w.y()
			};
		}
	utils_gpu_available constexpr utils::math::vec2f cdiv(utils::math::vec2f z, utils::math::vec2f w) noexcept
		{
		return cmul(z, utils::math::vec2f(w.x(), -w.y())) / utils::math::vec2f::dot(w, w);
		}

	utils_gpu_available constexpr simple_static_vector<float, 5> solveQuintic(float a, float b, float c, float d, float e, float f) noexcept
		{
		float p = (a * c * 5.f - b * b * 2.f) / (a * a * 5.f);
		float q = (a * a * d * 25.f - a * b * c * 15.f + b * b * b * 4.f) / (a * a * a * 25.f);
		float r = (a * a * a * e * 125.f - a * a * b * d * 50.f + a * b * b * c * 15.f - b * b * b * b * 3.f) / (a * a * a * a * 125.f);
		float s = (a * a * a * a * f * 3125.f - a * a * a * b * e * 625.f + a * a * b * b * d * 125.f - a * b * b * b * c * 25.f + b * b * b * b * b * 4.f) / (a * a * a * a * a * 3125.f);

		float bound = 1.f + utils::math::max(1.f, utils::math::max(utils::math::abs(p), utils::math::max(utils::math::abs(q), utils::math::max(utils::math::abs(r), utils::math::abs(s)))));
		//bound *= 0.414213562373; // Correction if perturbing with random([-1...1])
		bound *= 0.5;

		std::array<utils::math::vec2f, 5> roots
			{
			utils::math::vec2f{           bound,  0.f           }, 
			utils::math::vec2f{ 0.309016994375f,  0.951056516295f} * bound, 
			utils::math::vec2f{-0.809016994375f,  0.587785252292f} * bound, 
			utils::math::vec2f{-0.809016994375f, -0.587785252292f} * bound, 
			utils::math::vec2f{ 0.309016994375f, -0.951056516295f} * bound
			};

		for (size_t iter = 0; iter < 25; iter++)
			{
			float maxEval{-1e20f};
			for (size_t root = 0; root < 5; root++)
				{
				utils::math::vec2f z = roots[root];
				utils::math::vec2f quinticVal = cmul(cmul(cmul(cmul(z, z) + utils::math::vec2f(p, 0.f), z) + utils::math::vec2f(q, 0.f), z) + utils::math::vec2f(r, 0.f), z) + utils::math::vec2f(s, 0.f);
				maxEval = utils::math::max(maxEval, utils::math::max(utils::math::abs(quinticVal.x()), utils::math::abs(quinticVal.y())));

				utils::math::vec2f denom = z - roots[(root + 1) % 5];
				denom = cmul(denom, z - roots[(root + 2) % 5]);
				denom = cmul(denom, z - roots[(root + 3) % 5]);
				denom = cmul(denom, z - roots[(root + 4) % 5]);

				roots[root] -= cdiv(quinticVal, denom);
				}

			if (maxEval < 1e-7) break;
			}

		simple_static_vector<float, 5> ret;

		float offs = b / (5.f * a);
		for (int root = 0; root < 5; root++)
			{
			utils::math::vec2f z = roots[root];
			if (utils::math::abs(z.y()) < 1e-7)
				{
				ret.push_back(z.x() - offs);
				}
			}

		return ret;
		}

	utils_gpu_available constexpr float dot2(utils::math::vec2f v) noexcept { return utils::math::vec2f::dot(v, v); }

	simple_static_vector<float, 5> solved_quintic(const utils::math::vec2f point, const utils::math::vec2f v1, const utils::math::vec2f v2, const utils::math::vec2f v3, const utils::math::vec2f v4) noexcept
		{
		using ::operator*; //TODO understand why `side * float` in `return_types` namespace hides `vec<float, extent> * float` that's declared in global namespace

		// Convert to power basis
		const utils::math::vec2f a = v4 + (v2 - v3) * 3.f - v1;
		const utils::math::vec2f b = (v1 - v2 * 2.f + v3) * 3.f;
		const utils::math::vec2f c = (v2 - v1) * 3.f;
		const utils::math::vec2f d = v1 - point;

		// Quintic coefficients (derivative of distance-for-t with 2 factored out)
		const float qa = 3.f * utils::math::vec2f::dot(a, a);
		const float qb = 5.f * utils::math::vec2f::dot(a, b);
		const float qc = 4.f * utils::math::vec2f::dot(a, c) + 2.f * utils::math::vec2f::dot(b, b);
		const float qd = 3.f * (utils::math::vec2f::dot(b, c) + utils::math::vec2f::dot(d, a));
		const float qe = utils::math::vec2f::dot(c, c) + 2.f * utils::math::vec2f::dot(d, b);
		const float qf = utils::math::vec2f::dot(d, c);

		closest_precalculated_information_t closest;

		const auto quintic_roots = solveQuintic(qa, qb, qc, qd, qe, qf);
		return quintic_roots;
		}
	}
























namespace utils::math::geometry::sdf::details::bezier::_4pt::free_functions
	{
	using ::operator*; //TODO understand why `side * float` in `return_types` namespace hides `vec<float, extent> * float` that's declared in global namespace
	using ::operator-; //TODO understand why `side * float` in `return_types` namespace hides `vec<float, extent> * float` that's declared in global namespace
	using ::operator/; //TODO understand why `side * float` in `return_types` namespace hides `vec<float, extent> * float` that's declared in global namespace
	using vec2 = utils::math::vec2f;
	using vec3 = utils::math::vec3f;
	using vec4 = utils::math::vec4f;
	using utils::math::max;
	using utils::math::min;
	using utils::math::clamp;
	using std::pow;

	inline static constexpr float very_large_number{utils::math::constants::finf};
	//inline static constexpr float very_large_number{1e38f};

	utils::math::vec<float, 3> operator*(float value, const utils::math::vec<float, 3>& vec)
		{
		utils::math::vec<float, 3> ret;
		for (size_t i = 0; i < 3; i++)
			{
			ret[i] = vec[i] * value;
			}
		return ret;
		}
	template <size_t size>
	utils::math::vec<float, size> operator*(float value, const utils::math::vec<float, size>& vec)
		{
		utils::math::vec<float, size> ret;
		for (size_t i = 0; i < size; i++)
			{
			ret[i] = vec[i] * value;
			}
		return ret;
		}
	template <size_t size>
	utils::math::vec<float, size> operator-(float value, const utils::math::vec<float, size>& vec)
		{
		utils::math::vec<float, size> ret;
		for (size_t i = 0; i < size; i++)
			{
			ret[i] = value - vec[i];
			}
		return ret;
		}
	template <size_t size>
	utils::math::vec<float, size> operator/(float value, const utils::math::vec<float, size>& vec)
		{
		utils::math::vec<float, size> ret;
		for (size_t i = 0; i < size; i++)
			{
			ret[i] = value / vec[i];
			}
		return ret;
		}
	template <size_t size>
	utils::math::vec<float, size> operator-(const utils::math::vec<float, size>& vec)
		{
		utils::math::vec<float, size> ret;
		for (size_t i = 0; i < size; i++)
			{
			ret[i] = -vec[i];
			}
		return ret;
		}

	/*
	Exact distance to cubic bezier curve by computing roots of the derivative(s)
	to isolate roots of a fifth degree polynomial and Halley's Method to compute them.
	Inspired by https://www.shadertoy.com/view/4sXyDr and https://www.shadertoy.com/view/ldXXWH
	See also my approximate version:
	https://www.shadertoy.com/view/lsByRG
	*/
	const float eps = .000005;
	const float zoom = 1.f;
	const float dot_size = .005;
	const vec3 point_col = vec3(1.f, 1.f, 0.f);
	const int halley_iterations = 8;

	//lagrange positive real root upper bound
	//see for example: https://doi.org/10.1016/j.jsc.2014.09.038
	float upper_bound_lagrange5(float a0, float a1, float a2, float a3, float a4) {

		vec4 coeffs1 = vec4(a0, a1, a2, a3);

		vec4 neg1 = max(-coeffs1, vec4(0.f));
		float neg2 = max(-a4, 0.f);

		const vec4 indizes1 = vec4(0.f, 1.f, 2.f, 3.f);
		const float indizes2 = 4.f;

		vec4 bounds1 = pow(neg1, 1.f / (5.f - indizes1));
		float bounds2 = pow(neg2, 1.f / (5.f - indizes2));

		vec2 min1_2 = min(bounds1.xz(), bounds1.yw());
		vec2 max1_2 = max(bounds1.xz(), bounds1.yw());

		float maxmin = max(min1_2.x(), min1_2.y());
		float minmax = min(max1_2.x(), max1_2.y());

		float max3 = max(max1_2.x(), max1_2.y());

		float max_max = max(max3, bounds2);
		float max_max2 = max(min(max3, bounds2), max(minmax, maxmin));

		return max_max + max_max2;
		}

		//lagrange upper bound applied to f(-x) to get lower bound
	float lower_bound_lagrange5(float a0, float a1, float a2, float a3, float a4) {

		vec4 coeffs1 = vec4(-a0, a1, -a2, a3);

		vec4 neg1 = max(-coeffs1, vec4(0.f));
		float neg2 = max(-a4, 0.f);

		const vec4 indizes1 = vec4(0.f, 1.f, 2.f, 3.f);
		const float indizes2 = 4.f;

		vec4 bounds1 = pow(neg1, 1.f / (5.f - indizes1));
		float bounds2 = pow(neg2, 1.f / (5.f - indizes2));

		vec2 min1_2 = min(bounds1.xz(), bounds1.yw());
		vec2 max1_2 = max(bounds1.xz(), bounds1.yw());

		float maxmin = max(min1_2.x(), min1_2.y());
		float minmax = min(max1_2.x(), max1_2.y());

		float max3 = max(max1_2.x(), max1_2.y());

		float max_max = max(max3, bounds2);
		float max_max2 = max(min(max3, bounds2), max(minmax, maxmin));

		return -max_max - max_max2;
		}

	vec2 parametric_cub_bezier(float t, vec2 p0, vec2 p1, vec2 p2, vec2 p3) {
		vec2 a0 = (-p0 + 3.f * p1 - 3.f * p2 + p3);
		vec2 a1 = (3.f * p0 - 6.f * p1 + 3.f * p2);
		vec2 a2 = (-3.f * p0 + 3.f * p1);
		vec2 a3 = p0;

		return (((a0 * t) + a1) * t + a2) * t + a3;
		}

	void sort_roots3(utils::math::vecref3f roots) {
		vec3 tmp;

		tmp[0] = min(roots[0], min(roots[1], roots[2]));
		tmp[1] = max(roots[0], min(roots[1], roots[2]));
		tmp[2] = max(roots[0], max(roots[1], roots[2]));

		roots = tmp;
		}

	void sort_roots4(utils::math::vecref4f roots) {
		vec4 tmp;

		vec2 min1_2 = min(roots.xz(), roots.yw());
		vec2 max1_2 = max(roots.xz(), roots.yw());

		float maxmin = max(min1_2.x(), min1_2.y());
		float minmax = min(max1_2.x(), max1_2.y());

		tmp[0] = min(min1_2.x(), min1_2.y());
		tmp[1] = min(maxmin, minmax);
		tmp[2] = max(minmax, maxmin);
		tmp[3] = max(max1_2.x(), max1_2.y());

		roots = tmp;
		}

	float eval_poly5(float a0, float a1, float a2, float a3, float a4, float x) {

		float f = ((((x + a4) * x + a3) * x + a2) * x + a1) * x + a0;

		return f;
		}

		//halley's method
		//basically a variant of newton raphson which converges quicker and has bigger basins of convergence
		//see http://mathworld.wolfram.com/HalleysMethod.html
		//or https://en.wikipedia.org/wiki/Halley%27s_method
	float halley_iteration5(float a0, float a1, float a2, float a3, float a4, float x) {

		float f = ((((x + a4) * x + a3) * x + a2) * x + a1) * x + a0;
		float f1 = (((5.f * x + 4.f * a4) * x + 3.f * a3) * x + 2.f * a2) * x + a1;
		float f2 = ((20.f * x + 12.f * a4) * x + 6.f * a3) * x + 2.f * a2;

		return x - (2.f * f * f1) / (2.f * f1 * f1 - f * f2);
		}

	float halley_iteration4(vec4 coeffs, float x) {

		float f = (((x + coeffs[3]) * x + coeffs[2]) * x + coeffs[1]) * x + coeffs[0];
		float f1 = ((4.f * x + 3.f * coeffs[3]) * x + 2.f * coeffs[2]) * x + coeffs[1];
		float f2 = (12.f * x + 6.f * coeffs[3]) * x + 2.f * coeffs[2];

		return x - (2.f * f * f1) / (2.f * f1 * f1 - f * f2);
		}

		// Modified from http://tog.acm.org/resources/GraphicsGems/gems/Roots3And4.c
		// Credits to Doublefresh for hinting there
	int solve_quadric(vec2 coeffs, utils::math::vecref2f roots) {

		// normal form: x^2 + px + q = 0
		float p = coeffs[1] / 2.f;
		float q = coeffs[0];

		float D = p * p - q;

		if (D < 0.f) {
			return 0;
			}
		else if (D > 0.f) {
			roots[0] = -sqrt(D) - p;
			roots[1] = sqrt(D) - p;

			return 2;
			}
		}

		//From Trisomie21
		//But instead of his cancellation fix i'm using a newton iteration
	int solve_cubic(vec3 coeffs, utils::math::vecref3f r) {

		float a = coeffs[2];
		float b = coeffs[1];
		float c = coeffs[0];

		float p = b - a * a / 3.0f;
		float q = a * (2.0f* a * a - 9.0f* b) / 27.0f + c;
		float p3 = p * p * p;
		float d = q * q + 4.0f* p3 / 27.0f;
		float offset = -a / 3.0f;
		if (d >= 0.0f) { // Single solution
			float z = sqrt(d);
			float u = (-q + z) / 2.0;
			float v = (-q - z) / 2.0;
			u = sign(u) * pow(abs(u), 1.0f/ 3.0f);
			v = sign(v) * pow(abs(v), 1.0f/ 3.0f);
			r[0] = offset + u + v;

			//Single newton iteration to account for cancellation
			float f = ((r[0] + a) * r[0] + b) * r[0] + c;
			float f1 = (3.f * r[0] + 2.f * a) * r[0] + b;

			r[0] -= f / f1;

			return 1;
			}
		float u = sqrt(-p / 3.0f);
		float v = acos(-sqrt(-27.0f/ p3) * q / 2.0f) / 3.0f;
		float m = cos(v), n = sin(v) * 1.732050808f;

		//Single newton iteration to account for cancellation
		//(once for every root)
		r[0] = offset + u * (m + m);
		r[1] = offset - u * (n + m);
		r[2] = offset + u * (n - m);

		vec3 f = ((r + a) * r + b) * r + c;
		vec3 f1 = (3.f * r + 2.f * a) * r + b;

		r -= f / f1;

		return 3;
		}

		// Modified from http://tog.acm.org/resources/GraphicsGems/gems/Roots3And4.c
		// Credits to Doublefresh for hinting there
	int solve_quartic(vec4 coeffs, utils::math::vecref4f s) {

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

		//	vec3 cubic_coeffs;

		//	cubic_coeffs[0] = q;
		//	cubic_coeffs[1] = p;
		//	cubic_coeffs[2] = 0.f;

		//	num = solve_cubic(cubic_coeffs, s.xyz);

		//	s[num] = 0.f;
		//	num++;
		//}
		{
			/* solve the resolvent cubic ...f */

		vec3 cubic_coeffs;

		cubic_coeffs[0] = 1.0f/ 2.f * r * p - 1.0f/ 8.f * q * q;
		cubic_coeffs[1] = -r;
		cubic_coeffs[2] = -1.0f/ 2.f * p;

		solve_cubic(cubic_coeffs, s.xyz());

		/* ...f and take the one real solution ...f */

		float z = s[0];

		/* ...f to build two quadric equations */

		float u = z * z - r;
		float v = 2.f * z - p;

		if (u > -eps) {
			u = sqrt(abs(u));
			}
		else {
			return 0;
			}

		if (v > -eps) {
			v = sqrt(abs(v));
			}
		else {
			return 0;
			}

		vec2 quad_coeffs;

		quad_coeffs[0] = z - u;
		quad_coeffs[1] = q < 0.f ? -v : v;

		num = solve_quadric(quad_coeffs, s.xy());

		quad_coeffs[0] = z + u;
		quad_coeffs[1] = q < 0.f ? v : -v;

		vec2 tmp = vec2(very_large_number);
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
		for (int i = 0; i < 4; i += 2) {
			if (i < num) {
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
	float cubic_bezier_sign(vec2 uv, vec2 p0, vec2 p1, vec2 p2, vec2 p3) {

		float cu = (-p0.y() + 3.f * p1.y() - 3.f * p2.y() + p3.y());
		float qu = (3.f * p0.y() - 6.f * p1.y() + 3.f * p2.y());
		float li = (-3.f * p0.y() + 3.f * p1.y());
		float co = p0.y() - uv.y();

		vec3 roots = vec3(very_large_number);
		int n_roots = solve_cubic(vec3(co / cu, li / cu, qu / cu), roots);

		int n_ints = 0;

		for (int i = 0; i < 3; i++) {
			if (i < n_roots) {
				if (roots[i] >= 0.f && roots[i] <= 1.f) {
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

		vec2 tang1 = p0.xy() - p1.xy();
		vec2 tang2 = p2.xy() - p3.xy();

		vec2 nor1 = vec2(tang1.y(), -tang1.x());
		vec2 nor2 = vec2(tang2.y(), -tang2.x());

		if (p0.y() < p1.y()) {
			if ((uv.y() <= p0.y()) && (vec2::dot(uv - p0.xy(), nor1) < 0.f)) {
				n_ints++;
				}
			}
		else {
			if (!(uv.y() <= p0.y()) && !(vec2::dot(uv - p0.xy(), nor1) < 0.f)) {
				n_ints++;
				}
			}

		if (p2.y() < p3.y()) {
			if (!(uv.y() <= p3.y()) && vec2::dot(uv - p3.xy(), nor2) < 0.f) {
				n_ints++;
				}
			}
		else {
			if ((uv.y() <= p3.y()) && !(vec2::dot(uv - p3.xy(), nor2) < 0.f)) {
				n_ints++;
				}
			}

		if (n_ints == 0 || n_ints == 2 || n_ints == 4) {
			return 1.f;
			}
		else {
			return -1.f;
			}
		}

		//float cubic_bezier_dis(vec2 uv, vec2 p0, vec2 p1, vec2 p2, vec2 p3)
	float cubic_bezier_t(utils::math::vec2f uv, utils::math::vec2f p0, utils::math::vec2f p1, utils::math::vec2f p2, utils::math::vec2f p3) noexcept
		{

		//switch points when near to end point to minimize numerical error
		//only needed when control point(s) very far away
		#if 0
		vec2 mid_curve = parametric_cub_bezier(.5, p0, p1, p2, p3);
		vec2 mid_points = (p0 + p3) / 2.f;

		vec2 tang = mid_curve - mid_points;
		vec2 nor = vec2(tang.y(), -tang.x());

		if (sign(dot(nor, uv - mid_curve)) != sign(dot(nor, p0 - mid_curve))) {
			vec2 tmp = p0;
			p0 = p3;
			p3 = tmp;

			tmp = p2;
			p2 = p1;
			p1 = tmp;
			}
		#endif

		vec2 a3 = (-p0 + 3.f * p1 - 3.f * p2 + p3);
		vec2 a2 = (3.f * p0 - 6.f * p1 + 3.f * p2);
		vec2 a1 = (-3.f * p0 + 3.f * p1);
		vec2 a0 = p0 - uv;

		//compute polynomial describing distance to current pixel dependent on a parameter t
		float bc6 = vec2::dot(a3, a3);
		float bc5 = 2.f * vec2::dot(a3, a2);
		float bc4 = vec2::dot(a2, a2) + 2.f * vec2::dot(a1, a3);
		float bc3 = 2.f * (vec2::dot(a1, a2) + vec2::dot(a0, a3));
		float bc2 = vec2::dot(a1, a1) + 2.f * vec2::dot(a0, a2);
		float bc1 = 2.f * vec2::dot(a0, a1);
		float bc0 = vec2::dot(a0, a0);

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

		vec4 c1 = vec4(b1, 2.f * b2, 3.f * b3, 4.f * b4) / 5.f;
		vec3 c2 = vec3(c1[1], 2.f * c1[2], 3.f * c1[3]) / 4.f;
		vec2 c3 = vec2(c2[1], 2.f * c2[2]) / 3.f;
		float c4 = c3[1] / 2.f;

		vec4 roots_drv = vec4(very_large_number);

		int num_roots_drv = solve_quartic(c1, roots_drv);
		sort_roots4(roots_drv);

		float ub = upper_bound_lagrange5(b0, b1, b2, b3, b4);
		float lb = lower_bound_lagrange5(b0, b1, b2, b3, b4);

		vec3 a = vec3(very_large_number);
		vec3 b = vec3(very_large_number);

		vec3 roots = vec3(very_large_number);

		int num_roots = 0;

		//compute root isolating intervals by roots of derivative and outer root bounds
		//only roots going form - to + considered, because only those result in a minimum
		if (num_roots_drv == 4) {
			if (eval_poly5(b0, b1, b2, b3, b4, roots_drv[0]) > 0.f) {
				a[0] = lb;
				b[0] = roots_drv[0];
				num_roots = 1;
				}

			if (sign(eval_poly5(b0, b1, b2, b3, b4, roots_drv[1])) != sign(eval_poly5(b0, b1, b2, b3, b4, roots_drv[2]))) {
				if (num_roots == 0) {
					a[0] = roots_drv[1];
					b[0] = roots_drv[2];
					num_roots = 1;
					}
				else {
					a[1] = roots_drv[1];
					b[1] = roots_drv[2];
					num_roots = 2;
					}
				}

			if (eval_poly5(b0, b1, b2, b3, b4, roots_drv[3]) < 0.f) {
				if (num_roots == 0) {
					a[0] = roots_drv[3];
					b[0] = ub;
					num_roots = 1;
					}
				else if (num_roots == 1) {
					a[1] = roots_drv[3];
					b[1] = ub;
					num_roots = 2;
					}
				else {
					a[2] = roots_drv[3];
					b[2] = ub;
					num_roots = 3;
					}
				}
			}
		else {
			if (num_roots_drv == 2) {
				if (eval_poly5(b0, b1, b2, b3, b4, roots_drv[0]) < 0.f) {
					num_roots = 1;
					a[0] = roots_drv[1];
					b[0] = ub;
					}
				else if (eval_poly5(b0, b1, b2, b3, b4, roots_drv[1]) > 0.f) {
					num_roots = 1;
					a[0] = lb;
					b[0] = roots_drv[0];
					}
				else {
					num_roots = 2;

					a[0] = lb;
					b[0] = roots_drv[0];

					a[1] = roots_drv[1];
					b[1] = ub;
					}

				}
			else {//num_roots_drv==0
				vec3 roots_snd_drv = vec3(very_large_number);
				int num_roots_snd_drv = solve_cubic(c2, roots_snd_drv);

				vec2 roots_trd_drv = vec2(very_large_number);
				int num_roots_trd_drv = solve_quadric(c3, roots_trd_drv);
				num_roots = 1;

				a[0] = lb;
				b[0] = ub;
				}

				//further subdivide intervals to guarantee convergence of halley's method
				//by using roots of further derivatives
			vec3 roots_snd_drv = vec3(very_large_number);
			int num_roots_snd_drv = solve_cubic(c2, roots_snd_drv);
			sort_roots3(roots_snd_drv);

			int num_roots_trd_drv = 0;
			vec2 roots_trd_drv = vec2(very_large_number);

			if (num_roots_snd_drv != 3) {
				num_roots_trd_drv = solve_quadric(c3, roots_trd_drv);
				}

			for (int i = 0; i < 3; i++) {
				if (i < num_roots) {
					for (int j = 0; j < 3; j += 2) {
						if (j < num_roots_snd_drv) {
							if (a[i] < roots_snd_drv[j] && b[i] > roots_snd_drv[j]) {
								if (eval_poly5(b0, b1, b2, b3, b4, roots_snd_drv[j]) > 0.f) {
									b[i] = roots_snd_drv[j];
									}
								else {
									a[i] = roots_snd_drv[j];
									}
								}
							}
						}
					for (int j = 0; j < 2; j++) {
						if (j < num_roots_trd_drv) {
							if (a[i] < roots_trd_drv[j] && b[i] > roots_trd_drv[j]) {
								if (eval_poly5(b0, b1, b2, b3, b4, roots_trd_drv[j]) > 0.f) {
									b[i] = roots_trd_drv[j];
									}
								else {
									a[i] = roots_trd_drv[j];
									}
								}
							}
						}
					}
				}
			}

		float closest_distance{very_large_number};
		float closest_t{0.f};

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
				roots[i] = clamp(roots[i], 0.f, 1.f);
				vec2 to_curve = uv - parametric_cub_bezier(roots[i], p0, p1, p2, p3);

				float candidate_distance{vec2::dot(to_curve, to_curve)};
				if (candidate_distance < closest_distance)
					{
					closest_distance = candidate_distance;
					closest_t = roots[i];
					}

				//d0 = min(d0, dot(to_curve, to_curve));
				}
			}

		return closest_t;
		}

	}