import matplotlib.pyplot as plt

class Level:
    def __init__(self, energy, spin_parity, band_name, index_in_band):
        self.energy = energy
        self.spin_parity = spin_parity
        self.band_name = band_name
        self.index_in_band = index_in_band

class Transition:
    def __init__(self, from_level, to_level, energy, strength):
        self.from_level = from_level
        self.to_level = to_level
        self.energy = energy
        self.strength = strength

def input_levels():
    levels = []
    index_counter = {}
    print("请输入能级信息（输入空行结束）：")
    print("格式：带名 激发能(keV) 自旋宇称(如 2+)")
    while True:
        line = input("> ").strip()
        if not line:
            break
        try:
            band, energy, spin_parity = line.split()
            energy = float(energy)
            if band not in index_counter:
                index_counter[band] = 0
            lvl = Level(energy, spin_parity, band, index_counter[band])
            index_counter[band] += 1
            levels.append(lvl)
        except ValueError:
            print("格式错误，请重新输入")
    return levels

def input_transitions(levels):
    print("请输入跃迁信息（输入空行结束）：")
    print("格式：起始能级能量(keV) 终止能级能量(keV) 跃迁能量(keV) 强度")
    transitions = []
    energy_map = {lvl.energy: lvl for lvl in levels}
    while True:
        line = input("> ").strip()
        if not line:
            break
        try:
            e_from, e_to, energy, strength = map(float, line.split())
            if e_from in energy_map and e_to in energy_map:
                tr = Transition(energy_map[e_from], energy_map[e_to], energy, strength)
                transitions.append(tr)
            else:
                print("找不到匹配的能级，请确认激发能")
        except ValueError:
            print("格式错误，请重新输入")
    return transitions

def draw_chart(levels, transitions):
    band_names = list({lvl.band_name for lvl in levels})
    band_positions = {band: i * 2 for i, band in enumerate(band_names)}
    band_width = 1.0

    fig, ax = plt.subplots(figsize=(10, 6))
    ax.set_xlim(-1, len(band_names) * 2)
    ax.set_ylim(-100, max(lvl.energy for lvl in levels) + 200)
    ax.axis('off')

    for lvl in levels:
        x = band_positions[lvl.band_name] + lvl.index_in_band * 0.2
        y = lvl.energy
        ax.hlines(y, x, x + band_width, color='black')
        ax.text(x, y + 15, f"{lvl.energy} keV", ha='left', va='bottom', fontsize=8)
        ax.text(x + band_width, y + 15, lvl.spin_parity, ha='right', va='bottom', fontsize=8)

    for tr in transitions:
        x1 = band_positions[tr.from_level.band_name] + tr.from_level.index_in_band * 0.2 + band_width / 2
        y1 = tr.from_level.energy
        x2 = band_positions[tr.to_level.band_name] + tr.to_level.index_in_band * 0.2 + band_width / 2
        y2 = tr.to_level.energy
        ax.annotate(f"{tr.energy} keV", xy=((x1 + x2)/2, (y1 + y2)/2),
                    xytext=(0, 5), textcoords='offset points',
                    ha='center', fontsize=7)
        ax.annotate('', xy=(x2, y2), xytext=(x1, y1),
                    arrowprops=dict(arrowstyle='->', lw=tr.strength))

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    levels = input_levels()
    transitions = input_transitions(levels)
    draw_chart(levels, transitions)