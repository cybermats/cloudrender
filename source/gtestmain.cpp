#include <gtest/gtest.h>
#include "camera.h"
#include "config.h"
#include "intersection.h"
#include "triangle.h"

class CameraTestSimple : public ::testing::Test {
protected:
  virtual void SetUp() {
  vec3f position(0, 0, 0);
  vec3f up(0, 1, 0);
  vec3f lookat(0, 0, 1);
  float focal = 0.5;
  float hfov = 0.25;
  float fstop = 1;
  cam = new camera_t(position, up, lookat, focal, hfov, fstop, nullptr);
  }

  virtual void TearDown() {
    delete cam;
  }

  camera_t *cam;
};

class CameraTestOff : public ::testing::Test {
protected:
  virtual void SetUp() {
  vec3f position(0, 0, 1);
  vec3f up(0, 1, 0);
  vec3f lookat(0, 0, 0);
  float focal = 0.5;
  float hfov = 0.1;
  float fstop = 1;
  cam = new camera_t(position, up, lookat, focal, hfov, fstop, nullptr);
  }

  virtual void TearDown() {
    delete cam;
  }

  camera_t *cam;
};

::testing::AssertionResult compare_vec(const char* m_expr, const char* n_expr,
					 const vec3f& a, const vec3f& b) {
  if ((a-b).length() < config::ERR)
    return ::testing::AssertionSuccess();

  return ::testing::AssertionFailure()
    << m_expr << " and " << n_expr << " (" << a << " and " << b << ") "
    << "are not the same.";
}

::testing::AssertionResult compare_vec(const char* m_expr, const char* n_expr,
					 const vec2f& a, const vec2f& b) {
  if ((a-b).length() < config::ERR)
    return ::testing::AssertionSuccess();

  return ::testing::AssertionFailure() << 
    "\t" << m_expr << " = (" << a << ")" << std::endl <<
    "\t" << n_expr << " = (" << b << ")" << std::endl <<
    "are not the same.";
}

TEST_F(CameraTestSimple, settings) {
  float exp_f = 0.5f;
  ASSERT_FLOAT_EQ(exp_f, cam->f());
}

TEST_F(CameraTestSimple, lens_eq_center_center) {
  auto orig = vec3f(0, 0, 1);
  auto pos = vec3f(0.0, 0.0, 0);

  auto dir = (pos - orig).normalize();
  auto r = ray_t{{},dir,{},0};
  auto i = intersection_t{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(0, 0);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}

TEST_F(CameraTestSimple, lens_eq_off_center) {
  auto orig = vec3f(0, 0, 1);
  auto pos = vec3f(0.1, 0.1, 0);

  auto dir = (pos - orig).normalize();
  auto r = ray_t{{},dir,{},0};
  auto i = intersection_t{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(0, 0);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}

TEST_F(CameraTestSimple, lens_eq_center_off) {
  auto orig = vec3f(0.01, 0.01, 1);
  auto pos = vec3f(0, 0, 0);

  auto dir = (pos - orig).normalize();
  auto r = ray_t{{},dir,{},0};
  auto i = intersection_t{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(0.01, -0.01);
  ASSERT_EQ(cam->d_i(), 1);
  ASSERT_EQ(cam->d_o(), 1);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}

TEST_F(CameraTestSimple, lens_eq_off_off) {
  auto orig = vec3f(0, 0.01, 1);
  auto pos = vec3f(0.1, 0.1, 0);

  auto dir = (pos - orig).normalize();
  auto r = ray_t{{},dir,{},0};
  auto i = intersection_t{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(0, -0.01);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}

TEST_F(CameraTestOff, lens_eq_center_center) {
  auto orig = vec3f(0, 0, 0);
  auto pos = vec3f(0, 0, 1);

  auto dir = (pos - orig).normalize();
  auto r = ray_t{{},dir,{},0};
  auto i = intersection_t{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(0, 0);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}

TEST_F(CameraTestOff, lens_eq_off_center) {
  auto orig = vec3f(0, 0, 0);
  auto pos = vec3f(0.1, 0.1, 1);

  auto dir = (pos - orig).normalize();
  auto r = ray_t{{},dir,{},0};
  auto i = intersection_t{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(0, 0);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}

TEST_F(CameraTestOff, lens_eq_center_off) {
  auto orig = vec3f(0.01, 0.01, 0);
  auto pos = vec3f(0, 0, 1);

  auto dir = (pos - orig).normalize();
  auto r = ray_t{{},dir,{},0};
  auto i = intersection_t{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(-0.01, -0.01);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}

TEST_F(CameraTestOff, lens_eq_off_off) {
  auto orig = vec3f(0.01, 0.01, 0);
  auto pos = vec3f(0.1, 0.1, 1);

  auto dir = (pos - orig).normalize();
  auto r = ray_t{{},dir,{},0};
  auto i = intersection_t{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(-0.01, -0.01);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}

TEST(Triangle, Midpoint) {
  vec3f v0(0, 1, 0);
  vec3f v1(0, 1, 1);
  vec3f v2(1, 1, 1);
  triangle_t tri(v0, v1, v2, nullptr);

  vec3f exp(1.f/3.f, 1, 2.f/3.f);
  vec3f act = tri.midpoint();
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
