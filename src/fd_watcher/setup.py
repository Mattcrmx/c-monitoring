"""Setup for cython extension."""

from Cython.Build import cythonize
from setuptools import Extension, setup

extensions = [
    Extension(
        name="descriptors",
        sources=["descriptors.pyx", "core/monitor.c", "core/utils.c", "core/api.c"],
    )
]

setup(
    name="descriptors",
    ext_modules=cythonize(extensions, compiler_directives={"language_level": "3str"}),
)
