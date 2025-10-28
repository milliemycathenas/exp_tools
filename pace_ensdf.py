import paceENSDF as pe
import json

e = pe.ENSDF()
edata = e.load_ensdf()
# edata = e.load_pace()

# print("所有 parentID（母核）：")
# for d in edata:
#     print(d.get('parentID'))


nuclide = None
for d in edata:
    if d.get('parentID') == 'Cs128':
        nuclide = d
        break
    
# if nuclide:
#     print("字段列表：", nuclide.keys())
#     # 打印全部内容看看有无 levels 和 decayGammas
#     print(json.dumps(nuclide, indent=2))
# else:
#     print("未找到 126Cs 数据")

# print(nuclide.keys())

if nuclide:
    for level in nuclide.get('levelScheme', []):
        for gamma in level.get("gammaDecay", []):
            print(
                f"Eg = {gamma.get('gammaEnergy')} keV, "
                f"multi = {gamma.get('multipolarity')}, "
                f"ratio = {gamma.get('mixingRatio')}, "
                f"intensity = {gamma.get('gammaIntensity')}"
            )

# levels = cs126['levels']          
# gammas = cs126['decayGammas']  

# print("excited energies")
# for lvl in levels:
#     print(f"  {lvl['energy']} keV  Jπ={lvl.get('spin', '?')}")

# # 打印 gamma 跃迁
# print("\ngamma transitions")
# for g in gammas:
#     print(f"  {g['fromLevel']} → {g['toLevel']}    Eg = {g['Egamma']} keV")