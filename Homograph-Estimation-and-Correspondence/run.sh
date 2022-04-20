python -B main.py --img1_path "scene/book1.jpg" --img2_path "scene/scene.jpg" --method "sift" --save_path "result/book1/sift"
python -B main.py --img1_path "scene/book2.jpg" --img2_path "scene/scene.jpg" --method "sift" --save_path "result/book2/sift"
python -B main.py --img1_path "scene/book3.jpg" --img2_path "scene/scene.jpg" --method "sift" --save_path "result/book3/sift"

python -B main.py --img1_path "scene/book1.jpg" --img2_path "scene/scene.jpg" --method "surf" --save_path "result/book1/surf"
python -B main.py --img1_path "scene/book2.jpg" --img2_path "scene/scene.jpg" --method "surf" --save_path "result/book2/surf"
python -B main.py --img1_path "scene/book3.jpg" --img2_path "scene/scene.jpg" --method "surf" --save_path "result/book3/surf"

python -B main.py --img1_path "scene/book1.jpg" --img2_path "scene/scene.jpg" --handcraft --save_path "result/book1/handcraft"
python -B main.py --img1_path "scene/book2.jpg" --img2_path "scene/scene.jpg" --handcraft --save_path "result/book2/handcraft"
python -B main.py --img1_path "scene/book3.jpg" --img2_path "scene/scene.jpg" --handcraft --save_path "result/book3/handcraft"