#pragma once

#include "ShaderFlat.h"

class CScene;

class CShaderPhong : public CShaderFlat
{
public:
	/**
	 * @brief Constructor
	 * @param scene Reference to the scene
	 * @param color The color of the object
	 * @param ka The ambient coefficient
	 * @param kd The diffuse reflection coefficients
	 * @param ks The specular refelection coefficients
	 * @param ke The shininess exponent
	 */
	CShaderPhong(CScene& scene, Vec3f color, float ka, float kd, float ks, float ke)
		: CShaderFlat(color)
		, m_scene(scene)
		, m_ka(ka)
		, m_kd(kd)
		, m_ks(ks)
		, m_ke(ke)
	{}
	virtual ~CShaderPhong(void) = default;

	virtual Vec3f Shade(const Ray& ray) const override
	{
		// --- PUT YOUR CODE HERE ---
		//const int SHADOW_RAYS = 1;
		const int SHADOW_RAYS = 20;
		Vec3f acolor = m_ka * CShaderFlat::Shade(ray);
		Vec3f diffusion_sum = 0;
		Ray lray;
		for (int i = 0;i < m_scene.m_vpLights.size(); i++){
			lray.org = ray.org + ray.t * ray.dir;
			for (int shadow_rays = 0; shadow_rays < SHADOW_RAYS; shadow_rays++){
				std::optional<Vec3f> lRadiance = m_scene.m_vpLights[i]->Illuminate(lray);
				lray.t = std::numeric_limits<float>::infinity();
				if (!m_scene.Occluded(lray)){
					if(lRadiance){
						float theta = max(0.0f, lray.dir.dot(ray.hit->GetNormal(ray)));
						diffusion_sum += * lRadiance * theta;
	
					}
				}
			}
		}

		diffusion_sum /= SHADOW_RAYS;
		Vec3f dcolor = m_kd * diffusion_sum.mul(CShaderFlat::Shade(ray));

		Vec3f specular_sum = 0;
		Ray iray;
		for(int i = 0;i < m_scene.m_vpLights.size(); i++){
			iray.org = ray.org + ray.t * ray.dir;
			for(int shadow_rays = 0; shadow_rays < SHADOW_RAYS; shadow_rays++){
				std::optional<Vec3f> lRadiance = m_scene.m_vpLights[i]->Illuminate(iray);
				iray.t = std::numeric_limits<float>::infinity();
				if (!m_scene.Occluded(lray)){
					if (lRadiance){
						Vec3f reflected_distance = iray.dir - 2 * (iray.dir.dot(ray.hit->GetNormal(ray))) * ray.hit->GetNormal(ray);
						float theta = max(0.0f, ray.dir.dot(reflected_distance));
						specular_sum += * lRadiance * pow(theta, m_ke);
					}
				}
			}
		}
		specular_sum /= SHADOW_RAYS;
		Vec3f scolor = m_ks * RGB(0,0,0).mul(specular_sum);

		return acolor + dcolor + scolor;
	}


	
private:
	CScene& m_scene;
	float 	m_ka;    ///< ambient coefficient
	float 	m_kd;    ///< diffuse reflection coefficients
	float 	m_ks;    ///< specular refelection coefficients
	float 	m_ke;    ///< shininess exponent
};