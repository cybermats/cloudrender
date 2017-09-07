#include <gtest/gtest.h>
#include "camera.h"
#include "config.h"
#include "intersection.h"

class CameraTestSimple : public ::testing::Test {
protected:
  virtual void SetUp() {
  vec3f position(0, 0, 0);
  vec3f up(0, 1, 0);
  vec3f lookat(0, 0, 1);
  float od = 1;
  float id = 1;
  float fstop = 2.2;
  cam = new camera(position, up, lookat, od, id, fstop, nullptr);
  }

  virtual void TearDown() {
    delete cam;
  }

  camera *cam;
};

class CameraTestOff : public ::testing::Test {
protected:
  virtual void SetUp() {
  vec3f position(0, 0, 1);
  vec3f up(0, 1, 0);
  vec3f lookat(0, 0, 0);
  float od = 1;
  float id = 1;
  float fstop = 2.2;
  cam = new camera(position, up, lookat, od, id, fstop, nullptr);
  }

  virtual void TearDown() {
    delete cam;
  }

  camera *cam;
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
  auto r = ray{{},dir,{},0};
  auto i = intersection{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(0, 0);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}

TEST_F(CameraTestSimple, lens_eq_off_center) {
  auto orig = vec3f(0, 0, 1);
  auto pos = vec3f(0.1, 0.1, 0);

  auto dir = (pos - orig).normalize();
  auto r = ray{{},dir,{},0};
  auto i = intersection{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(0, 0);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}

TEST_F(CameraTestSimple, lens_eq_center_off) {
  auto orig = vec3f(0.01, 0.01, 1);
  auto pos = vec3f(0, 0, 0);

  auto dir = (pos - orig).normalize();
  auto r = ray{{},dir,{},0};
  auto i = intersection{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(0.01, -0.01);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}

TEST_F(CameraTestSimple, lens_eq_off_off) {
  auto orig = vec3f(0, 0.01, 1);
  auto pos = vec3f(0.1, 0.1, 0);

  auto dir = (pos - orig).normalize();
  auto r = ray{{},dir,{},0};
  auto i = intersection{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(0, -0.01);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}

TEST_F(CameraTestOff, lens_eq_center_center) {
  auto orig = vec3f(0, 0, 0);
  auto pos = vec3f(0, 0, 1);

  auto dir = (pos - orig).normalize();
  auto r = ray{{},dir,{},0};
  auto i = intersection{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(0, 0);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}

TEST_F(CameraTestOff, lens_eq_off_center) {
  auto orig = vec3f(0, 0, 0);
  auto pos = vec3f(0.1, 0.1, 1);

  auto dir = (pos - orig).normalize();
  auto r = ray{{},dir,{},0};
  auto i = intersection{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(0, 0);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}

TEST_F(CameraTestOff, lens_eq_center_off) {
  auto orig = vec3f(0.01, 0.01, 0);
  auto pos = vec3f(0, 0, 1);

  auto dir = (pos - orig).normalize();
  auto r = ray{{},dir,{},0};
  auto i = intersection{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(-0.01, -0.01);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}

TEST_F(CameraTestOff, lens_eq_off_off) {
  auto orig = vec3f(0.01, 0.01, 0);
  auto pos = vec3f(0.1, 0.1, 1);

  auto dir = (pos - orig).normalize();
  auto r = ray{{},dir,{},0};
  auto i = intersection{0, nullptr, {}, &r, pos};

  vec2f act;
  auto exp = vec2f(-0.01, -0.01);
  ASSERT_TRUE(cam->lens_equation(i, act));
  EXPECT_PRED_FORMAT2(compare_vec, act, exp);
}


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
